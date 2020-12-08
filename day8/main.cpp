#include <iostream>
#include "solution.h"
#include <algorithm>
#include <functional>


enum class operation {
    acc,
    nop,
    jmp
};

struct instruction {
    operation op;
    int argument;
    int flags;
};

std::ostream& operator<<(std::ostream& out, const instruction& i) {
    switch (i.op) {
        case operation::acc:
            out << "acc";
            break;
        case operation::nop:
            out << "nop";
            break;
        case operation::jmp:
            out << "jmp";
            break;
    }
    out << " " << i.argument << "\t(flags:" << i.flags << ")";
    return out;
}


class code_runner {
public:

    void push_instruction(const instruction& i) {
        program.push_back(i);
    }

    void reset() {
        ip = accumulator = 0;
        infinite_loop_detected = false;
        for (auto& i : program) {
            i.flags = 0;
        }
    }

    void run() {
        while (ip < program.size() && (program.at(ip).flags & 1) == 0) {
            program.at(ip).flags |= 1;
            tick();
        }
        if (ip < program.size()) {
            infinite_loop_detected = true;
        }
    }

    void tick() {
        const auto& i = program.at(ip);
//        std::cout << ip << ":\t\t" << i << std::endl;
        switch (i.op) {
            case operation::acc:
                accumulator += i.argument;
//                std::cout << "\taccumulator -> " << accumulator << std::endl;
                ip++;
                break;
            case operation::nop:
//                std::cout << "\tnop" << std::endl;
                ip++;
                break;
            case operation::jmp:
                ip += i.argument;
//                std::cout << "\tjmp to " << ip << std::endl;
                break;
        }
    }

    void dump() {
        int offset = 0;
        for (const auto& i : program) {
            std::cout << offset << ":\t\t" << i << std::endl;
            offset++;
        }
    }

    int part_1() {
        reset();
        run();
        return accumulator;
    }

    int part_2() {
        auto original_program = program;

        for (int i = 0; i < program.size(); i++) {
            if (original_program.at(i).op == operation::nop) {
                program = original_program;
                program[i].op = operation::jmp;
                reset();
                run();
                if (!infinite_loop_detected) return accumulator;
            } else if (original_program.at(i).op == operation::jmp) {
                program = original_program;
                program[i].op = operation::nop;
                reset();
                run();
                if (!infinite_loop_detected) return accumulator;
            } else {
                continue;
            }
        }
        std::cerr << "Should not get here" << std::endl;
        return -1;
    }

private:
    int ip{0};
    int accumulator{0};
    bool infinite_loop_detected{false};
    std::vector<instruction> program;
};


class day8 : public aoc::solution {
public:

    instruction parse_instruction(std::string line) {
        auto pos = line.find_first_of(' ');
        auto mnemonic = aoc::trim(line.substr(0, pos));
        auto argument = std::stoi(aoc::trim(line.substr(pos + 1)));
        return instruction{
                .op = operation_from_mnemonic(mnemonic),
                .argument = argument,
                .flags = 0
        };
    }

    operation operation_from_mnemonic(const std::string& mnemonic) {
        if (mnemonic == "acc")
            return operation::acc;
        if (mnemonic == "jmp")
            return operation::jmp;
        return operation::nop;
    }

    void run(std::istream& in, std::ostream& out) override {
        code_runner runner{};
        for (std::string line; std::getline(in, line);) {
            runner.push_instruction(parse_instruction(aoc::trim(line)));
        }
//        runner.dump();
        out << runner.part_1() << std::endl;
        out << runner.part_2() << std::endl;
    }

protected:
    void register_tests() override {
        add_test(aoc::test{
                .name = "First",
                .input = "nop +0\n"
                         "acc +1\n"
                         "jmp +4\n"
                         "acc +3\n"
                         "jmp -3\n"
                         "acc -99\n"
                         "acc +1\n"
                         "jmp -4\n"
                         "acc +6",
                .expected_output = "5\n8"
        });
    }
};

int main(int argc, char** argv) {
    day8 solution;
    return solution.execute(argc, argv);
}
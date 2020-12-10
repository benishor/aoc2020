#include <iostream>
#include "solution.h"
#include <functional>
#include <utility>

enum class operation {
    acc,
    nop,
    jmp
};

struct instruction {
    operation op;
    int argument;
    int flags;

    void visit() {
        flags |= 1;
    }

    [[nodiscard]] bool is_visited() const {
        return flags & 1;
    }

    void clear_visit() {
        flags &= ~1;
    }
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

using program = std::vector<instruction>;

class computer {
public:

    computer(std::vector<instruction> program, bool debug)
            : debug(debug), pgm(std::move(program)) {
        reset();
    }

    void reset() {
        ip = accumulator = 0;
        infinite_loop_detected = halted = false;
        for (auto& i : pgm) {
            i.clear_visit();
        }
    }

    void run() {
        while (!halted) {
            tick();
        }
    }

    void tick() {
        if (ip >= pgm.size()) {
            halted = true;
            if (debug)
                std::cout << "\t\tprogram halt" << std::endl;
            return;
        }

        auto& i = pgm.at(ip);
        if (i.is_visited()) {
            halted = true;
            infinite_loop_detected = true;
            if (debug)
                std::cout << "\t\tinfinite loop detected" << std::endl;
            return;
        }

        i.visit();

        if (debug)
            std::cout << ip << ":\t\t" << i << std::endl;

        switch (i.op) {
            case operation::acc:
                accumulator += i.argument;
                ip++;
                break;
            case operation::nop:
                ip++;
                break;
            case operation::jmp:
                ip += i.argument;
                break;
        }
        if (debug) {
            std::cout << "\tacc\t-> " << accumulator << std::endl;
            std::cout << "\tip\t-> " << ip << std::endl;
        }
    }

    void dump() {
        int offset = 0;
        for (const auto& i : pgm) {
            std::cout << offset << ":\t\t" << i << std::endl;
            offset++;
        }
    }

    [[nodiscard]] bool is_infinite_loop_detected() const {
        return infinite_loop_detected;
    }

    [[nodiscard]] bool is_halted() const {
        return halted;
    }

    [[nodiscard]] int get_accumulator() const {
        return accumulator;
    }

private:
    bool debug{false};
    size_t ip{0};
    int accumulator{0};
    bool infinite_loop_detected{false};
    bool halted{false};
    program pgm;
};


class day8 : public aoc::solution {
public:

    void run(std::istream& in, std::ostream& out) override {
        program pgm = read_program(in);

        out << part_1(pgm) << std::endl;
        out << part_2(pgm) << std::endl;
    }

    static program read_program(std::istream& in) {
        program pgm;
        for (std::string line; std::getline(in, line);) {
            pgm.push_back(parse_instruction(aoc::trim(line)));
        }
        return pgm;
    }

    static instruction parse_instruction(const std::string& line) {
        auto pos = line.find_first_of(' ');
        auto mnemonic = aoc::trim(line.substr(0, pos));
        auto argument = std::stoi(aoc::trim(line.substr(pos + 1)));
        return instruction{
                .op = operation_from_mnemonic(mnemonic),
                .argument = argument,
                .flags = 0};
    }

    static operation operation_from_mnemonic(const std::string& mnemonic) {
        if (mnemonic == "acc")
            return operation::acc;
        else if (mnemonic == "jmp")
            return operation::jmp;
        else
            return operation::nop;
    }

    static int part_1(const program& pgm) {
        auto pc = computer{pgm, false};
        pc.reset();
        pc.run();
        return pc.get_accumulator();
    }

    static int part_2(const program& pgm) {
        for (size_t i = 0; i < pgm.size(); i++) {
            if (pgm.at(i).op == operation::nop) {
                program current_program = pgm;
                current_program[i].op = operation::jmp;
                auto pc = computer{current_program, false};
                pc.reset();
                pc.run();
                if (!pc.is_infinite_loop_detected()) return pc.get_accumulator();
            } else if (pgm.at(i).op == operation::jmp) {
                program current_program = pgm;
                current_program[i].op = operation::nop;
                auto pc = computer{current_program, false};
                pc.reset();
                pc.run();
                if (!pc.is_infinite_loop_detected()) return pc.get_accumulator();
            } else {
                continue;
            }
        }
        std::cerr << "Should not get here" << std::endl;
        return -1;
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
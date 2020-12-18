#include "solution.h"
#include "expression_evaluator.h"

int part1_operator_precedence(char op) {
    switch (op) {
        case '+':
        case '-':
        case '*':
        case '/':
            return 1;
        default:
            return 0;
    }
}

int part2_operator_precedence(char op) {
    switch (op) {
        case '+':
        case '-':
            return 2;
        case '*':
        case '/':
            return 1;
        default:
            return 0;
    }
}

class day18 : public aoc::solution {
protected:

    void run(std::istream& in, std::ostream& out) override {
        expression_evaluator<long> calculator_part1{part1_operator_precedence};
        expression_evaluator<long> calculator_part2{part2_operator_precedence};

        long part_1{0}, part_2{0};
        for (std::string line; std::getline(in, line);) {
            line = aoc::trim(line);
            part_1 += calculator_part1.evaluate(line);
            part_2 += calculator_part2.evaluate(line);
        }
        out << part_1 << std::endl;
        out << part_2 << std::endl;
    }

    void register_tests() override {
        add_test(aoc::test{
                .name = "First",
                .input = "1 + 2 * 3 + 4 * 5 + 6",
                .expected_output = "71\n231"
        });
        add_test(aoc::test{
                .name = "Second",
                .input = "1+(2*3)+(4*(5+6))",
                .expected_output = "51\n51"
        });
    }
};

int main(int argc, char** argv) {
    day18 solution{};
    return solution.execute(argc, argv);
}

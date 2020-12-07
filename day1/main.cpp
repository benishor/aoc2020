#include <iostream>
#include "solution.h"
#include <algorithm>

class day1 : public aoc::solution {
public:
    static void part1(const std::vector<int>& numbers, std::ostream& out) {
        for (int i = 0; i < numbers.size() - 1; i++) {
            for (int j = i + 1; j < numbers.size(); j++) {
                auto a = numbers[i], b = numbers[j];
                if (a + b == 2020) {
                    out << (a * b) << std::endl;
                    return;
                }
            }
        }
    }

    static void part2(const std::vector<int>& numbers, std::ostream& out) {
        for (int i = 0; i < numbers.size() - 2; i++) {
            for (int j = i + 1; j < numbers.size() - 1; j++) {
                for (int k = j + 1; k < numbers.size(); k++) {
                    auto a = numbers[i], b = numbers[j], c = numbers[k];
                    if (a + b + c == 2020) {
                        out << (a * b * c) << std::endl;
                        return;
                    }
                }
            }
        }
    }

    void run(std::istream& in, std::ostream& out) override {
        auto numbers = aoc::elements_from_stream<int>(in);
//        fmt::print("Numbers: {}", fmt::join(numbers, ","));
        part1(numbers, out);
        part2(numbers, out);
    }

    void register_tests() override {
        add_test({
                         .name = "Test1",
                         .input = "1721\n"
                                  "979\n"
                                  "366\n"
                                  "299\n"
                                  "675\n"
                                  "1456",
                         .expected_output = "514579\n241861950"
                 });
    }
};

int main(int argc, char** argv) {
    day1 solution;
    return solution.execute(argc, argv);
}

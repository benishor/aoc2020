#include <iostream>
#include "solution.h"
#include <algorithm>
#include <functional>

class day6 : public aoc::solution {
public:

    void run(std::istream& in, std::ostream& out) override {

        std::map<char, int> group_questions;
        int group_members = 0;
        auto part_1 = 0;
        auto part_2 = 0;
        for (std::string line; std::getline(in, line);) {
            if (aoc::trim(line).empty()) {
                part_1 += group_questions.size();
                for (auto kv : group_questions) {
                    if (kv.second == group_members) {
                        part_2++;
                    }
                }
                group_questions.clear();
                group_members = 0;
            } else {
                group_members++;
                for (char c : aoc::trim(line)) {
                    if (group_questions.count(c)) {
                        group_questions[c]++;
                    } else {
                        group_questions[c] = 1;
                    }
                }
            }
        }
        if (!group_questions.empty()) {
            part_1 += group_questions.size();
            for (auto kv : group_questions) {
                if (kv.second == group_members) {
                    part_2++;
                }
            }
        }

        std::cout << part_1 << std::endl;
        std::cout << part_2 << std::endl;
    }
};

int main(int argc, char** argv) {
    day6 solution;
    return solution.execute(argc, argv);
}
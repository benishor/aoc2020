#include <iostream>
#include "solution.h"
#include <algorithm>
#include <functional>

class day5 : public aoc::solution {
public:

    unsigned int id_from_string(std::string& text) {
        unsigned int id = 0;
        for (char c : text) {
            id <<= 1;
            if (c == 'B' || c == 'R') {
                id |= 1;
            }
        }
        return id;
    }

    void run(std::istream& in, std::ostream& out) override {
        auto max_id = 0;
        std::map<unsigned int, bool> seats_taken;
        for (std::string line; std::getline(in, line);) {
            auto id = id_from_string(aoc::trim(line));
            seats_taken[id] = true;
            if (id > max_id) {
                max_id = id;
            }
        }
        out << max_id << std::endl;

        auto my_seat = 0;
        for (int i = 0b0000000101; i < 0b1111110110; i++) {
            auto current_seat_exists = seats_taken.count(i);
            if (!current_seat_exists) {
                auto prev_seat_exists = seats_taken.count(i - 1);
                auto next_seat_exists = seats_taken.count(i + 1);
                if (prev_seat_exists && next_seat_exists) {
                    my_seat = i;
                    break;
                }
            }
        }
        out << my_seat << std::endl;

    }

    void register_tests() override {
        add_test({
                         .name = "First",
                         .input = "BFFFBBFRRR",
                         .expected_output = "567"
                 });
    }
};

int main(int argc, char** argv) {
    day5 solution;
    return solution.execute(argc, argv);
}
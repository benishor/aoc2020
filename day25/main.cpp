#include "solution.h"

using element_type = unsigned long long;

class day25 : public aoc::solution {
protected:

    void run(std::istream& in, std::ostream& out) override {
        std::string line;

        std::getline(in, line);
        element_type card_public_key = std::stol(line);

        std::getline(in, line);
        element_type door_public_key = std::stol(line);

        element_type card_loop_size = find_loop_size_for(card_public_key);
        out << transform(door_public_key, card_loop_size) << std::endl;
    }

    static element_type find_loop_size_for(element_type key) {
        element_type loop_index = 0;
        element_type value = 1;
        while (value != key) {
            value = (value * 7UL) % 20201227;
            loop_index++;
        }
        return loop_index;
    }

    static element_type transform(element_type subject, element_type loop_size) {
        element_type value = 1;
        while (loop_size--) {
            value = (value * subject) % 20201227;
        }
        return value;
    }

    void register_tests() override {
        add_test(aoc::test{
                .name = "First",
                .input = "5764801\n"
                         "17807724",
                .expected_output = "14897079"
        });
    }
};

int main(int argc, char** argv) {
    auto solution = day25{};
    return solution.execute(argc, argv);
}

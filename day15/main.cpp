#include "solution.h"

using element_type = long;

class day15 : public aoc::solution {
protected:

    void run(std::istream& in, std::ostream& out) override {
        auto elements = aoc::csv_elements_from_stream<element_type>(in);
        element_type round = 1;
        element_type last_element = 0;
        std::map<element_type, std::deque<element_type>> last_rounds;
        for (const auto& el : elements) {
            last_rounds[el].push_back(round);
            last_element = el;
            round++;
        }

        while (round <= 30000000) {
            if (round == 2021) {
                out << last_element << std::endl;
                out.flush();
            }

            bool first_time_spoken = last_rounds[last_element].size() < 2;
            if (first_time_spoken) {
                last_element = 0;
            } else {
                last_element = last_rounds[last_element].at(1) - last_rounds[last_element].at(0);
            }
            last_rounds[last_element].push_back(round);
            if (last_rounds[last_element].size() > 2) {
                last_rounds[last_element].pop_front();
            }
            round++;
        }
        out << last_element << std::endl;
    }

    void register_tests() override {
        add_test(aoc::test{
                .name = "First",
                .input = "0,3,6",
                .expected_output = "436\n175594"
        });
    }
};

int main(int argc, char** argv) {
    day15 solution{};
    return solution.execute(argc, argv);
}

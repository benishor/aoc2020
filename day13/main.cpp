#include <numeric>
#include "solution.h"

using element_type = unsigned long long;

class day13 : public aoc::solution {
protected:
    void run(std::istream& in, std::ostream& out) override {
        std::string line;
        std::getline(in, line);
        int earliest_leave = std::stoi(aoc::trim(line));

        std::getline(in, line);
        std::stringstream ss{aoc::trim(line)};
        element_type earliest_bus_id = 0;
        element_type minutes_to_wait = std::numeric_limits<element_type>::max();
        std::map<element_type, element_type> offset_to_bus;
        int offset = 0;
        while (std::getline(ss, line, ',')) {
            if (line != "x") {
                int bus_id = std::stoi(line);
                offset_to_bus[offset] = bus_id;
                if ((earliest_leave % bus_id) == 0) {
                    earliest_bus_id = bus_id;
                    minutes_to_wait = 0;
                } else {
                    auto current_minutes_to_wait = bus_id - (earliest_leave % bus_id);
                    if (current_minutes_to_wait < minutes_to_wait) {
                        minutes_to_wait = current_minutes_to_wait;
                        earliest_bus_id = bus_id;
                    }
                }
            }
            offset++;
        }
        element_type part_1 = earliest_bus_id * minutes_to_wait;
        out << part_1 << std::endl;

        // part 2
        element_type timestamp = 0;
        element_type increment = 1;
        for (auto& kv : offset_to_bus) {
            auto bus_offset = kv.first;
            auto bus_frequency = kv.second;
            while (((timestamp + bus_offset) % bus_frequency) != 0) {
                timestamp += increment;
            }
            increment *= bus_frequency;
        }
        out << timestamp << std::endl;
    }

    void register_tests() override {
        add_test(aoc::test{
                .name = "First",
                .input = "939\n"
                         "7,13,x,x,59,x,31,19",
                .expected_output= "295"
        });

        add_test(aoc::test{
                .name = "Second",
                .input = "939\n"
                         "1789,37,47,1889",
                .expected_output = "aa\n1202161486"
        });
    }

};

int main(int argc, char** argv) {
    day13 solution{};
    return solution.execute(argc, argv);
}

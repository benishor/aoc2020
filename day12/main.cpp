#include "solution.h"

class day12 : public aoc::solution {
protected:

    static void rotate_clockwise(long& x, long& y) {
        auto new_x = y;
        auto new_y = -x;
        x = new_x;
        y = new_y;
    }

    static void rotate_counter_clockwise(long& x, long& y) {
        for (int i = 0; i < 3; i++) {
            rotate_clockwise(x, y);
        }
    }

    void run(std::istream& in, std::ostream& out) override {
        long x{0}, y{0}, dx{1}, dy{0};
        long x2{0}, y2{0}, waypoint_x{10}, waypoint_y{-1};
        for (std::string line; std::getline(in, line);) {
            switch (line.at(0)) {
                case 'F': {
                    int amount = std::stoi(aoc::trim(line).substr(1));
                    x += dx * amount;
                    y += dy * amount;
                    x2 += waypoint_x * amount;
                    y2 += waypoint_y * amount;
                    break;
                }
                case 'L': {
                    int amount = std::stoi(aoc::trim(line).substr(1)) / 90;
                    for (int i = 0; i < amount; i++) {
                        rotate_clockwise(dx, dy);
                        rotate_clockwise(waypoint_x, waypoint_y);
                    }
                    break;
                }
                case 'R': {
                    int amount = std::stoi(aoc::trim(line).substr(1)) / 90;
                    for (int i = 0; i < amount; i++) {
                        rotate_counter_clockwise(dx, dy);
                        rotate_counter_clockwise(waypoint_x, waypoint_y);
                    }
                    break;
                }
                case 'E': {
                    int amount = std::stoi(aoc::trim(line).substr(1));
                    x += amount;
                    waypoint_x += amount;
                    break;
                }
                case 'W': {
                    int amount = std::stoi(aoc::trim(line).substr(1));
                    x -= amount;
                    waypoint_x -= amount;
                    break;
                }
                case 'N': {
                    int amount = std::stoi(aoc::trim(line).substr(1));
                    y -= amount;
                    waypoint_y -= amount;
                    break;
                }
                case 'S': {
                    int amount = std::stoi(aoc::trim(line).substr(1));
                    y += amount;
                    waypoint_y += amount;
                    break;
                }
                default:
                    break;
            }
        }

        out << (std::abs(x) + std::abs(y)) << std::endl;
        out << (std::abs(x2) + std::abs(y2)) << std::endl;
    }

    void register_tests() override {
        add_test(aoc::test{
                .name = "First",
                .input = "F10\n"
                         "N3\n"
                         "F7\n"
                         "R90\n"
                         "F11",
                .expected_output = "25\n286"
        });
    }
};


int main(int argc, char** argv) {
    day12 solution{};
    return solution.execute(argc, argv);
}

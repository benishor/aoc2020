#include <iostream>
#include "solution.h"
#include <algorithm>
#include <numeric>

class day3 : public aoc::solution {
public:
    void run(std::istream& in, std::ostream& out) override {
        // part 1
        auto map = read_map(in);
        out << tree_encounters(map, 3, 1) << std::endl;

        // part 2
        std::vector<std::tuple<int, int>> paths = {
                {1, 1},
                {3, 1},
                {5, 1},
                {7, 1},
                {1, 2}
        };

        unsigned long product = std::accumulate(paths.begin(), paths.end(), 1L, [&](auto x, auto& path) {
            return x * tree_encounters(map, std::get<0>(path), std::get<1>(path));
        });
        out << product << std::endl;
    }

    static std::vector<std::string> read_map(std::istream& in) {
        std::vector<std::string> map;
        for (std::string line; std::getline(in, line);) {
            map.push_back(aoc::trim(line));
        }
        return map;
    }

    static unsigned long tree_encounters(const std::vector<std::string>& map, int slope_x, int slope_y) {
        auto width = map[0].size();
        auto height = map.size();
        auto x = 0;
        auto y = 0;
        auto trees_encountered = 0;
        while (y < height) {
            if (map[y][x] == '#') {
                trees_encountered++;
            }

            x = (x + slope_x) % width;
            y = y + slope_y;
        }
        return trees_encountered;
    }

    void register_tests() override {
        add_test({
                         .name = "First",
                         .input = "..##.......\n"
                                  "#...#...#..\n"
                                  ".#....#..#.\n"
                                  "..#.#...#.#\n"
                                  ".#...##..#.\n"
                                  "..#.##.....\n"
                                  ".#.#.#....#\n"
                                  ".#........#\n"
                                  "#.##...#...\n"
                                  "#...##....#\n"
                                  ".#..#...#.#",
                         .expectedOutput = "7\n336"
                 });
    }
};

int main(int argc, char** argv) {
    day3 solution;
    return solution.execute(argc, argv);
}
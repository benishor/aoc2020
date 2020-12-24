#include "solution.h"

enum class direction {
    east,
    west,
    north_east,
    north_west,
    south_east,
    south_west
};

namespace std {
    template<typename T1, typename T2>
    struct less<std::pair<T1, T2>> {
        bool operator()(const std::pair<T1, T2>& l,
                        const std::pair<T1, T2>& r) const {
            if (l.first == r.first)
                return l.second > r.second;

            return l.first > r.first;
        }
    };
}


class day24 : public aoc::solution {
protected:

    static bool is_tile_black(std::map<std::pair<int, int>, bool>& tiles, int x, int y) {
        auto key = std::make_pair(x, y);
        return tiles.count(key) && tiles[key];
    }

    static void set_tile_white(std::map<std::pair<int, int>, bool>& tiles, int x, int y) {
        auto key = std::make_pair(x, y);
        tiles[key] = false;
    }

    static void set_tile_black(std::map<std::pair<int, int>, bool>& tiles, int x, int y) {
        auto key = std::make_pair(x, y);
        tiles[key] = true;
    }

    int count_black_neighbours(std::map<std::pair<int, int>, bool>& tiles, int x, int y) {
        int result = 0;
        result += is_tile_black(tiles, x - 2, y);
        result += is_tile_black(tiles, x + 2, y);
        result += is_tile_black(tiles, x - 1, y - 1);
        result += is_tile_black(tiles, x + 1, y - 1);
        result += is_tile_black(tiles, x - 1, y + 1);
        result += is_tile_black(tiles, x + 1, y + 1);
        return result;
    }

    static bool are_coordinates_valid(int x, int y) {
        auto abs_x = abs(x);
        auto abs_y = abs(y);
        auto is_x_even = (abs_x & 1) == 0;
        auto is_y_even = (abs_y & 1) == 0;
        return (is_x_even && is_y_even) || (!is_x_even && !is_y_even);
    }

    void flip_tiles(std::map<std::pair<int, int>, bool>& tiles) {
        // establish bounds
        int min_x = std::numeric_limits<int>::max();
        int max_x = std::numeric_limits<int>::min();
        int min_y = std::numeric_limits<int>::max();
        int max_y = std::numeric_limits<int>::min();
        for (auto& kv : tiles) {
            if (kv.second) {
                if (kv.first.first < min_x) min_x = kv.first.first;
                if (kv.first.first > max_x) max_x = kv.first.first;
                if (kv.first.second < min_y) min_y = kv.first.second;
                if (kv.first.second > max_y) max_y = kv.first.second;
            }
        }

        min_x -= 2;
        max_x += 2;
        min_y -= 2;
        max_y += 2;

        auto new_tiles = tiles;
        for (int y = min_y; y <= max_y; y++) {
            for (int x = min_x; x <= max_x; x++) {
                if (!are_coordinates_valid(x, y)) continue;
                auto black_neighbours = count_black_neighbours(tiles, x, y);
                if (is_tile_black(tiles, x, y)) {
                    if (black_neighbours == 0 || black_neighbours > 2) {
                        set_tile_white(new_tiles, x, y);
                    }
                } else {
                    if (black_neighbours == 2) {
                        set_tile_black(new_tiles, x, y);
                    }
                }
            }
        }
        tiles = new_tiles;
    }

    void run(std::istream& in, std::ostream& out) override {
        std::map<std::pair<int, int>, bool> tiles;

        for (std::string line; std::getline(in, line);) {
            line = aoc::trim(line);
            std::vector<direction> directions{};
            for (size_t i = 0; i < line.size(); i++) {
                if (line.at(i) == 'n' && line.at(i + 1) == 'e') {
                    directions.push_back(direction::north_east);
                    i++;
                } else if (line.at(i) == 'n' && line.at(i + 1) == 'w') {
                    directions.push_back(direction::north_west);
                    i++;
                } else if (line.at(i) == 's' && line.at(i + 1) == 'e') {
                    directions.push_back(direction::south_east);
                    i++;
                } else if (line.at(i) == 's' && line.at(i + 1) == 'w') {
                    directions.push_back(direction::south_west);
                    i++;
                } else if (line.at(i) == 'e') {
                    directions.push_back(direction::east);
                } else if (line.at(i) == 'w') {
                    directions.push_back(direction::west);
                } else {
                    fmt::print(stderr, "Unknown direction {}", line.at(i));
                    exit(1);
                }
            }

            int tile_top_x{0}, tile_top_y{0};
            for (auto& d : directions) {
                switch (d) {
                    case direction::east:
                        tile_top_x += 2;
                        break;
                    case direction::west:
                        tile_top_x -= 2;
                        break;
                    case direction::north_east:
                        tile_top_x += 1;
                        tile_top_y -= 1;
                        break;
                    case direction::north_west:
                        tile_top_x -= 1;
                        tile_top_y -= 1;
                        break;
                    case direction::south_east:
                        tile_top_x += 1;
                        tile_top_y += 1;
                        break;
                    case direction::south_west:
                        tile_top_x -= 1;
                        tile_top_y += 1;
                        break;
                }
            }

            auto key = std::make_pair(tile_top_x, tile_top_y);
            if (tiles.count(key)) {
                tiles[key] = !tiles[key];
            } else {
                tiles[key] = true;
            }
        }

        int part_1 = count_black_tiles(tiles);
        out << part_1 << std::endl;

        for (int day = 0; day < 100; day++) {
            flip_tiles(tiles);
        }
        int part_2 = count_black_tiles(tiles);
        out << part_2 << std::endl;

    }

    int count_black_tiles(std::map<std::pair<int, int>, bool>& tiles) const {
        int part_2{0};
        for (auto& kv : tiles) {
            if (kv.second) {
                part_2++;
            }
        }
        return part_2;
    }

    void register_tests() override {
        add_test(aoc::test{
                .name = "First",
                .input = "sesenwnenenewseeswwswswwnenewsewsw\n"
                         "neeenesenwnwwswnenewnwwsewnenwseswesw\n"
                         "seswneswswsenwwnwse\n"
                         "nwnwneseeswswnenewneswwnewseswneseene\n"
                         "swweswneswnenwsewnwneneseenw\n"
                         "eesenwseswswnenwswnwnwsewwnwsene\n"
                         "sewnenenenesenwsewnenwwwse\n"
                         "wenwwweseeeweswwwnwwe\n"
                         "wsweesenenewnwwnwsenewsenwwsesesenwne\n"
                         "neeswseenwwswnwswswnw\n"
                         "nenwswwsewswnenenewsenwsenwnesesenew\n"
                         "enewnwewneswsewnwswenweswnenwsenwsw\n"
                         "sweneswneswneneenwnewenewwneswswnese\n"
                         "swwesenesewenwneswnwwneseswwne\n"
                         "enesenwswwswneneswsenwnewswseenwsese\n"
                         "wnwnesenesenenwwnenwsewesewsesesew\n"
                         "nenewswnwewswnenesenwnesewesw\n"
                         "eneswnwswnwsenenwnwnwwseeswneewsenese\n"
                         "neswnwewnwnwseenwseesewsenwsweewe\n"
                         "wseweeenwnesenwwwswnew",
                .expected_output = "10\n2208"
        });
    }
};

int main(int argc, char** argv) {
    auto solution = day24{};
    return solution.execute(argc, argv);
}

#include "solution.h"

struct position {
    int x, y, z, w;
};

bool operator<(const position& a, const position& b) {
    if (a.w == b.w) {
        if (a.z == b.z) {
            if (a.y == b.y) {
                return a.x < b.x;
            } else {
                return a.y < b.y;
            }
        } else {
            return a.z < b.z;
        }
    } else {
        return a.w < b.w;
    }
}

position operator+(const position& a, const position& b) {
    return position{
            a.x + b.x,
            a.y + b.y,
            a.z + b.z,
            a.w + b.w,
    };
}

position operator-(const position& a, const position& b) {
    return position{
            a.x - b.x,
            a.y - b.y,
            a.z - b.z,
            a.w - b.w,
    };
}

enum class dimensions {
    three_dee,
    four_dee
};

class pocket {
public:
    explicit pocket(dimensions mode_) : mode{mode_} {}

    [[nodiscard]] std::tuple<position, position> get_bounds() const {
        position minimum{std::numeric_limits<int>::max(),
                         std::numeric_limits<int>::max(),
                         std::numeric_limits<int>::max(),
                         std::numeric_limits<int>::max(),
        };
        position maximum{std::numeric_limits<int>::min(),
                         std::numeric_limits<int>::min(),
                         std::numeric_limits<int>::min(),
                         std::numeric_limits<int>::min(),
        };

        for (const auto& kv : cubes) {
            if (kv.second) {
                if (kv.first.x < minimum.x) {
                    minimum.x = kv.first.x;
                }
                if (kv.first.x > maximum.x) {
                    maximum.x = kv.first.x;
                }
                if (kv.first.y < minimum.y) {
                    minimum.y = kv.first.y;
                }
                if (kv.first.y > maximum.y) {
                    maximum.y = kv.first.y;
                }
                if (kv.first.z < minimum.z) {
                    minimum.z = kv.first.z;
                }
                if (kv.first.z > maximum.z) {
                    maximum.z = kv.first.z;
                }
                if (kv.first.w < minimum.w) {
                    minimum.w = kv.first.w;
                }
                if (kv.first.w > maximum.w) {
                    maximum.w = kv.first.w;
                }
            }
        }
        return std::make_tuple(minimum, maximum);
    }

    void set_active_cube(position p) {
        cubes[p] = true;
    }

    [[nodiscard]] inline bool cube_active_at(const position& p) const {
        return cubes.count(p) && cubes.at(p);
    }

    [[nodiscard]] int count_active_neighbours(const position& p) const {
        int result{0};

        int min_w = mode == dimensions::three_dee ? 0 : -1;
        int max_w = mode == dimensions::three_dee ? 0 : 1;

        for (int w = min_w; w <= max_w; w++) {
            for (int z = -1; z <= 1; z++) {
                for (int y = -1; y <= 1; y++) {
                    for (int x = -1; x <= 1; x++) {
                        if (w == 0 && z == 0 && y == 0 && x == 0) continue;
                        result += cube_active_at(position{p.x + x, p.y + y, p.z + z, p.w + w});
                    }
                }
            }
        }
        return result;
    }

    void tick() {
        std::map<position, bool> new_cubes;
        auto bounds = get_bounds();
        auto min = std::get<0>(bounds) - position{1, 1, 1, 1};
        auto max = std::get<1>(bounds) + position{1, 1, 1, 1};

        if (mode == dimensions::three_dee) {
            min.w = max.w = 0;
        }

        for (int w = min.w; w <= max.w; w++) {
            for (int z = min.z; z <= max.z; z++) {
                for (int y = min.y; y <= max.y; y++) {
                    for (int x = min.x; x <= max.x; x++) {
                        auto p = position{x, y, z, w};
                        bool is_cube_active = cube_active_at(p);
                        int neighbours = count_active_neighbours(p);
                        if ((is_cube_active && neighbours > 1 && neighbours < 4) ||
                            (!is_cube_active && neighbours == 3)) {
                            new_cubes[p] = true;
                        }
                    }
                }
            }
        }

        cubes = new_cubes;
    }

    [[maybe_unused]] void dump() const {
        auto bounds = get_bounds();
        auto min = std::get<0>(bounds);
        auto max = std::get<1>(bounds);
        for (int w = min.w; w <= max.w; w++) {
            for (int z = min.z; z <= max.z; z++) {
                std::cout << "w=" << w << ",z=" << z << std::endl;
                for (int y = min.y; y <= max.y; y++) {
                    for (int x = min.x; x <= max.x; x++) {
                        std::cout << (cube_active_at(position{x, y, z, w}) ? '#' : '.');
                    }
                    std::cout << std::endl;
                }
                std::cout << std::endl;
            }
        }
        std::cout << std::endl;
    }

    [[nodiscard]] size_t count_active_cubes() const {
        return std::count_if(cubes.begin(), cubes.end(), [](auto kv) { return kv.second == true; });
    }

    void set_mode(dimensions mode_) {
        this->mode = mode_;
    }

    void save_cubes() {
        backup = cubes;
    }

    void restore_cubes() {
        cubes = backup;
    }

private:
    dimensions mode;
    std::map<position, bool> cubes, backup;
};


class day17 : public aoc::solution {
protected:
    void run(std::istream& in, std::ostream& out) override {

        int y = 0;
        pocket p{dimensions::three_dee};
        for (std::string line; std::getline(in, line);) {
            line = aoc::trim(line);
            int x = 0;
            for (auto& c : line) {
                if (c == '#') {
                    p.set_active_cube(position{x, y, 0, 0});
                }
                x++;
            }
            y++;
        }

        // part 1
        p.save_cubes();
        for (int i = 0; i < 6; i++) {
            p.tick();
        }
        out << p.count_active_cubes() << std::endl;

        // part 2
        p.restore_cubes();
        p.set_mode(dimensions::four_dee);
        for (int i = 0; i < 6; i++) {
            p.tick();
        }
        out << p.count_active_cubes() << std::endl;
    }

    void register_tests() override {
        add_test(aoc::test{
                .name = "First",
                .input = ".#.\n"
                         "..#\n"
                         "###",
                .expected_output = "112\n848"
        });
    }
};

int main(int argc, char** argv) {
    day17 solution{};
    return solution.execute(argc, argv);
}

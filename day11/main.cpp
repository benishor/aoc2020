#include "solution.h"

enum class seat_type {
    empty,
    occupied,
    floor
};

class seat_map {
public:

    seat_map(const seat_map& rhs) = default;

    explicit seat_map(std::istream& in) :
            width(0), height(0) {
        for (std::string line; std::getline(in, line);) {
            auto l = aoc::trim(line);
            width = l.size();
            height++;
            for (const auto& c : l) {
                switch (c) {
                    case '#': {
                        data.push_back(seat_type::occupied);
                        break;
                    }
                    case 'L': {
                        data.push_back(seat_type::empty);
                        break;
                    }
                    case '.': {
                        data.push_back(seat_type::floor);
                        break;
                    }
                    default:
                        fmt::print(stderr, "Should not get here");
                        break;
                }
            }
        }
    }

    seat_type get(size_t x, size_t y) {
        return data[x + y * width];
    }

    void set(size_t x, size_t y, seat_type type) {
        data[x + y * width] = type;
    }

    size_t get_width() const {
        return width;
    }

    size_t get_height() const {
        return height;
    }

    bool run_round() {
        auto new_data = data;
        size_t offset = 0;
        bool changes_occurred = false;
        for (size_t y = 0; y < height; y++) {
            for (size_t x = 0; x < width; x++) {
                int adjacent_occupied_seats = 0;
                if (y > 0 && x > 0) adjacent_occupied_seats += data[offset - width - 1] == seat_type::occupied;
                if (y > 0) adjacent_occupied_seats += data[offset - width] == seat_type::occupied;
                if (y > 0 && x < width - 1) adjacent_occupied_seats += data[offset - width + 1] == seat_type::occupied;

                if (x > 0) adjacent_occupied_seats += data[offset - 1] == seat_type::occupied;
                if (x < width - 1) adjacent_occupied_seats += data[offset + 1] == seat_type::occupied;

                if (y < height - 1 && x > 0) adjacent_occupied_seats += data[offset + width - 1] == seat_type::occupied;
                if (y < height - 1) adjacent_occupied_seats += data[offset + width] == seat_type::occupied;
                if (y < height - 1 && x < width - 1)
                    adjacent_occupied_seats += data[offset + width + 1] == seat_type::occupied;

                if (data[offset] == seat_type::empty && adjacent_occupied_seats == 0) {
                    // If a seat is empty (L) and there are no occupied seats adjacent to it, the seat becomes occupied.
                    new_data[offset] = seat_type::occupied;
                    changes_occurred = true;
                } else if (data[offset] == seat_type::occupied && adjacent_occupied_seats >= 4) {
                    // If a seat is occupied (#) and four or more seats adjacent to it are also occupied, the seat becomes empty.
                    new_data[offset] = seat_type::empty;
                    changes_occurred = true;
                }

                offset++;
            }
        }
        data = new_data;
        return changes_occurred;
    }

    bool is_seat_occupied_on_direction(int x, int y, int dx, int dy) {
        int xx = x + dx;
        int yy = y + dy;
        while (xx >= 0 && xx < width && yy >= 0 && yy < height) {
            if (get(xx, yy) == seat_type::occupied) return true;
            if (get(xx, yy) == seat_type::empty) return false; // ?
            xx += dx;
            yy += dy;
        }
        return false;
    }

    bool run_round_2() {
        auto new_data = data;
        size_t offset = 0;
        bool changes_occurred = false;
        for (size_t y = 0; y < height; y++) {
            for (size_t x = 0; x < width; x++) {
                int adjacent_occupied_seats = 0;
                adjacent_occupied_seats += is_seat_occupied_on_direction(x, y, -1, -1);
                adjacent_occupied_seats += is_seat_occupied_on_direction(x, y, 0, -1);
                adjacent_occupied_seats += is_seat_occupied_on_direction(x, y, 1, -1);

                adjacent_occupied_seats += is_seat_occupied_on_direction(x, y, -1, 0);
                adjacent_occupied_seats += is_seat_occupied_on_direction(x, y, 1, 0);

                adjacent_occupied_seats += is_seat_occupied_on_direction(x, y, -1, 1);
                adjacent_occupied_seats += is_seat_occupied_on_direction(x, y, 0, 1);
                adjacent_occupied_seats += is_seat_occupied_on_direction(x, y, 1, 1);

                if (data[offset] == seat_type::empty && adjacent_occupied_seats == 0) {
                    // If a seat is empty (L) and there are no occupied seats adjacent to it, the seat becomes occupied.
                    new_data[offset] = seat_type::occupied;
                    changes_occurred = true;
                } else if (data[offset] == seat_type::occupied && adjacent_occupied_seats >= 5) {
                    // If a seat is occupied (#) and four or more seats adjacent to it are also occupied, the seat becomes empty.
                    new_data[offset] = seat_type::empty;
                    changes_occurred = true;
                }

                offset++;
            }
        }
        data = new_data;
        return changes_occurred;
    }

    void dump() {
        for (auto y = 0; y < height; y++) {
            for (auto x = 0; x < width; x++) {
                switch (get(x, y)) {
                    case seat_type::empty:
                        fmt::print("L");
                        break;
                    case seat_type::occupied:
                        fmt::print("#");
                        break;
                    case seat_type::floor:
                        fmt::print(".");
                        break;
                }
            }
            fmt::print("\n");
        }
    }

    long occupied_seats() {
        return std::count(data.begin(), data.end(), seat_type::occupied);
    }

private:
    size_t width;
    size_t height;
    std::vector<seat_type> data;
};

class day11 : public aoc::solution {
protected:
    void run(std::istream& in, std::ostream& out) override {
        seat_map map_1{in};
        auto map_2 = map_1;

        while (map_1.run_round());
        out << map_1.occupied_seats() << std::endl;

        while (map_2.run_round_2());
        out << map_2.occupied_seats() << std::endl;
    }

    void register_tests() override {
        add_test(aoc::test{
                .name = "First",
                .input = "L.LL.LL.LL\n"
                         "LLLLLLL.LL\n"
                         "L.L.L..L..\n"
                         "LLLL.LL.LL\n"
                         "L.LL.LL.LL\n"
                         "L.LLLLL.LL\n"
                         "..L.L.....\n"
                         "LLLLLLLLLL\n"
                         "L.LLLLLL.L\n"
                         "L.LLLLL.LL",
                .expected_output = "37\n26"
        });
    }
};

int main(int argc, char** argv) {
    day11 solution{};
    solution.execute(argc, argv);
    return 0;
}

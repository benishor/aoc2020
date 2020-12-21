#include <hwloc.h>
#include "solution.h"

template<std::size_t N>
void reverse(std::bitset<N>& b) {
    for (std::size_t i = 0; i < N / 2; ++i) {
        bool t = b[i];
        b[i] = b[N - i - 1];
        b[N - i - 1] = t;
    }
}

unsigned long hash(unsigned long nr) {
    auto bt = std::bitset<10>{nr};
    reverse(bt);
    auto nr2 = bt.to_ulong();
    return nr < nr2 ? nr : nr2;
}

const bool debug = false;

class tile {
public:
    explicit tile(long id_, const std::vector<std::string>& rows) : id{id_} {
        for (const auto& row : rows) {
            add_row(row);
        }
        precompute();
    }

    void set_unmatched_side(int i) {
        unmatched_sides[i] = true;
    }

    [[nodiscard]] bool is_side_unmatched(int i) const {
        return unmatched_sides[i];
    }

    void set_corner() {
        corner = true;
    }

    [[nodiscard]] bool is_corner() const {
        return corner;
    }

    void set_used() {
        used = true;
    }

    [[nodiscard]] bool is_used() const {
        return used;
    }

    // rotates 90deg, clockwise
    void rotate() {
        if (debug) std::cout << "\t- rotating " << id << std::endl;
        auto new_pixels = pixels;

        for (int i = 0; i < 10; i++) {
//            std::cout << "Setting pixel " << i << std::endl;
            std::bitset<10> bs{0};
            for (int j = 0; j < 10; j++) {
                std::bitset<10> bs2{pixels[9 - j]};
//                std::cout << "\t bit " << (9-j) << " from " << bs2 << "  is " << bs2[9-i] << std::endl;
                bs[9 - j] = bs2[9 - i];
            }
            new_pixels[i] = bs.to_ulong();
        }
        pixels = new_pixels;

        precompute();

        // side checksums
        auto new_unmatched_sides = unmatched_sides;
        auto tmp = unmatched_sides[3];
        for (int i = 3; i > 0; i--) {
            unmatched_sides[i] = unmatched_sides[i - 1];
        }
        unmatched_sides[0] = tmp;
    }

    void flip_vertically() {
        if (debug) std::cout << "\t- flipping vertically " << id << std::endl;
        auto new_pixels = pixels;
        for (int i = 0; i < 10; i++) {
            new_pixels[i] = pixels[9 - i];
        }
        pixels = new_pixels;

        precompute();

        auto tmp = unmatched_sides[0];
        unmatched_sides[0] = unmatched_sides[2];
        unmatched_sides[2] = tmp;
    }

    void flip_horizontally() {
        if (debug) std::cout << "\t- flipping horizontally " << id << std::endl;
        auto new_pixels = pixels;
        for (int i = 0; i < 10; i++) {
            auto bs = std::bitset<10>{pixels[i]};
            reverse(bs);
            new_pixels[i] = bs.to_ulong();
        }
        pixels = new_pixels;

        precompute();

        auto tmp = unmatched_sides[1];
        unmatched_sides[1] = unmatched_sides[3];
        unmatched_sides[3] = tmp;
    }

    void dump() {
        std::cout << "Tile " << id << std::endl;
        for (int i = 0; i < 10; i++) {
            std::cout << "\t" << std::bitset<10>{pixels[i]} << std::endl;
        }
        for (int i = 0; i < 4; i++) {
            std::cout << "sides[" << i << "] " << (unmatched_sides[i] ? '*' : ' ') << " = "
                      << std::bitset<10>{sides_checksum[i]} << " (" << sides_checksum[i] << ","
                      << hash(sides_checksum[i]) << ") " << std::endl;
        }
    }

    const long getId() const {
        return id;
    }

    [[nodiscard]] std::array<unsigned long, 4> getSidesChecksum() const {
        return sides_checksum;
    }

    bool get_pixel_at(size_t x, size_t y) {
        auto bs = std::bitset<10>{pixels[y]};
        return bs[9 - x];
    }

private:
    void add_row(const std::string& row) {
        std::bitset<10> row_as_bits{0};
        for (int i = 0; i < 10; i++) {
            row_as_bits[9 - i] = row.at(i) == '#';
        }
        pixels[rows_added++] = row_as_bits.to_ulong();
    }

    void precompute() {
        // top
        sides_checksum[0] = pixels[0];

        auto right = std::bitset<10>{0};
        auto left = std::bitset<10>{0};
        for (int i = 0; i < 10; i++) {
            left[i] = pixels[9 - i] & (1UL << 9);
            right[i] = pixels[9 - i] & (1UL << 0);
        }
        sides_checksum[1] = right.to_ulong();

        // bottom
        sides_checksum[2] = pixels[9];

        // left
        sides_checksum[3] = left.to_ulong();
    }


    const long id;
    int rows_added{0};
    std::array<unsigned long, 10> pixels{};
    std::array<unsigned long, 4> sides_checksum{0};
    std::array<bool, 4> unmatched_sides{false};
    bool corner{false};
    bool used{false};
};


class image {
public:
    explicit image(int side_) : side{side_} {
        data = std::vector<std::string>{size_t(side), std::string(side, ' ')};
    }

    void set_at(int x, int y, char value) {
        data[y][x] = value;
    }

    void flip_vertically() {
        for (int i = 0; i < data.size() / 2; i++) {
            auto tmp = data[i];
            data[i] = data[data.size() - 1 - i];
        }
    }

    void flip_horizontally() {
        for (auto& s : data) {
            std::reverse(s.begin(), s.end());
        }
    }

    // 90 deg, clockwise
    void rotate() {
        auto new_data = data;
        for (int y = 0; y < side; y++) {
            for (int x = 0; x < side; x++) {
                new_data[y][x] = data[side - 1 - x][side - 1 - y];
            }
        }
        data = new_data;
    }

    void dump() {
        for (auto& s : data)
            std::cout << s << std::endl;
    }

    std::tuple<int, int> count_nessies() const {
        static std::string pattern[3] = {"                  # ",
                                         "#    ##    ##    ###",
                                         " #  #  #  #  #  #   "};

        static int pattern_width = 20;
        static int pattern_height = 3;

        std::vector<std::tuple<int, int>> nessie_origins{};

        int result = 0;
        for (int y = 0; y < (side - pattern_height); y++) {
            for (int x = 0; x < (side - pattern_width); x++) {
                bool matches = true;
                for (int py = 0; py < pattern_height; py++) {
                    for (int px = 0; px < pattern_width; px++) {
                        if ((pattern[py][px] == '#') && data[y + py][x + px] != '#') {
                            matches = false;
                            goto break_loop;
                        }
                    }
                }
                break_loop:
                if (matches) {
                    nessie_origins.emplace_back(x, y);
                    result++;
                }
            }
        }

        int water_roughness = 0;
        for (int y = 0; y < side; y++) {
            for (int x = 0; x < side; x++) {
                if (data[y][x] == '#') {
                    bool part_of_a_nessie = false;
                    for (auto& origin: nessie_origins) {
                        for (int py = 0; py < pattern_height; py++) {
                            for (int px = 0; px < pattern_width; px++) {
                                if ((pattern[py][px] == '#') && (px + std::get<0>(origin) == x) &&
                                    ((py + std::get<1>(origin)) == y)) {
                                    part_of_a_nessie = true;
                                    goto end_nessie_search;
                                }
                            }
                        }
                    }
                    end_nessie_search:
                    if (!part_of_a_nessie) {
                        water_roughness++;
                    }
                }
            }
        }


        return std::make_tuple(result, water_roughness);
    }

    int side;
    std::vector<std::string> data{};
};

class frame {
public:
    explicit frame(int side_) : side{side_} {
        data = std::vector<tile*>(side * side, nullptr);
    }

    void dump_ids() {
        std::cout << std::endl;
        for (int y = 0; y < side; y++) {
            for (int x = 0; x < side; x++) {
                std::cout << get_at(x, y)->getId() << ",";
            }
            std::cout << std::endl;
        }
    }

    void dump_ascii() {
        std::cout << std::endl;
        for (size_t y = 0; y < side * 10; y++) {
            for (size_t x = 0; x < side * 10; x++) {
                auto tile_x = x / 10;
                auto tile_y = y / 10;
                auto tile = data[tile_x + tile_y * side];
                auto local_y = y % 10;
                auto local_x = x % 10;
                std::cout << (tile->get_pixel_at(local_x, local_y) ? '#' : '.');
                if (local_x == 9) std::cout << " ";
            }
            if ((y % 10) == 9) std::cout << std::endl;
            std::cout << std::endl;
        }
    }

    image get_image() {
        auto im = image{side * 8};

        int im_y = 0;
        for (size_t y = 0; y < side * 10; y++) {
            if ((y % 10) == 0 || (y % 10) == 9) continue;
            int im_x = 0;
            for (size_t x = 0; x < side * 10; x++) {
                auto tile_x = x / 10;
                auto tile_y = y / 10;
                auto tile = data[tile_x + tile_y * side];
                auto local_y = y % 10;
                auto local_x = x % 10;
                if (local_x > 0 && local_x < 9) {
                    im.set_at(im_x++, im_y, tile->get_pixel_at(local_x, local_y) ? '#' : '.');
                }
            }
            im_y++;
        }

        return im;
    }


    void set_at(int x, int y, tile* t) {
        data[x + y * side] = t;
    }

    tile* get_at(int x, int y) const {
        return data.at(x + y * side);
    }

    int side;
    std::vector<tile*> data{};
};

class day20 : public aoc::solution {
protected:
    static void remove_tile_from(tile* t, std::vector<tile*>& tiles) {
        auto it = tiles.begin();
        while (it != tiles.end()) {
            if ((*it)->getId() == t->getId()) {
                it = tiles.erase(it);
            } else {
                it++;
            }
        }
    }

    static void remove_tile_from_checksum_map(tile* t, std::map<unsigned long, std::vector<tile*>>& checksum_to_tile) {
        auto checksums = t->getSidesChecksum();
        for (int i = 0; i < 4; i++) {
            remove_tile_from(t, checksum_to_tile[hash(checksums[i])]);
        }
    }

    void run(std::istream& in, std::ostream& out) override {
        std::vector<tile*> tiles{};
        for (std::string line; std::getline(in, line);) {
            line = aoc::trim(line);
            if (line.empty()) continue;
            auto id = std::stol(line.substr(5, line.find_last_of(':')));
            std::vector<std::string> rows{};
            for (int i = 0; i < 10; i++) {
                std::getline(in, line);
                rows.push_back(aoc::trim(line));
            }
            tiles.push_back(new tile{id, rows});
        }

        std::map<unsigned long, std::vector<tile*>> checksum_to_tile{};
        for (auto& t : tiles) {
            auto checksums = t->getSidesChecksum();
            for (int i = 0; i < 4; i++) {
                checksum_to_tile[hash(checksums[i])].push_back(t);
            }
        }

        if (debug) {
            std::cout << "Side checksum to candidate tiles: " << std::endl;
            for (auto& kv : checksum_to_tile) {
                std::cout << "\tchecksum " << kv.first << ": ";
                for (auto& t : kv.second) {
                    std::cout << t->getId() << ",";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }

        unsigned long long part_1{1};
        for (auto& t : tiles) {
            auto checksums = t->getSidesChecksum();
            int how_many_unmatched_sides{0};
            for (int i = 0; i < 4; i++) {
                auto h = hash(checksums[i]);
                if (checksum_to_tile[h].size() == 1) {
                    how_many_unmatched_sides++;
                    t->set_unmatched_side(i);
                }
            }
            if (how_many_unmatched_sides > 1) {
                t->set_corner();
                part_1 *= t->getId();
            }
        }
        out << part_1 << std::endl;

        auto nr_tiles = tiles.size();
        auto side = int(sqrt(nr_tiles));
        frame f{side};

        std::map<unsigned long, tile*> tile_id_to_tile{};
        tile* first_corner = nullptr;
        for (auto& t : tiles) {
            if (t->is_corner()) {
                first_corner = t;
                break;
            }
        }

        // rotate first corner until unmatched sides are top and left
        while (!first_corner->is_side_unmatched(0) || !first_corner->is_side_unmatched(3)) {
            first_corner->rotate();
        }
        first_corner->set_used();
        remove_tile_from_checksum_map(first_corner, checksum_to_tile);

        f.set_at(0, 0, first_corner);

        for (int i = 1; i < side; i++) {
            auto prev = f.get_at(i - 1, 0);
            auto checksum_to_find = prev->getSidesChecksum()[1];

            if (debug) {
                std::cout << "Candidates for next tile: ";
                for (auto& c : checksum_to_tile[hash(checksum_to_find)]) {
                    std::cout << c->getId() << ",";
                }
                std::cout << std::endl;
            }

            auto candidate = checksum_to_tile[hash(checksum_to_find)][0];
            while (hash(candidate->getSidesChecksum()[3]) != hash(prev->getSidesChecksum()[1])) {
                candidate->rotate();
            }

            if (candidate->getSidesChecksum()[3] != prev->getSidesChecksum()[1]) {
                if (debug) std::cout << "need to flip vertically " << candidate->getId() << std::endl;
                candidate->flip_vertically();
            }

            candidate->set_used();
            f.set_at(i, 0, candidate);
            remove_tile_from_checksum_map(candidate, checksum_to_tile);
        }
        if (debug) std::cout << std::endl;

        for (int y = 1; y < side; y++) {
            for (int i = 0; i < side; i++) {
                auto tile_above = f.get_at(i, y - 1); // the one above
                auto checksum_to_find = tile_above->getSidesChecksum()[2]; // bottom side

                if (debug)
                    std::cout << "y = " << y << ", x = " << i << ", tile above: " << tile_above->getId() << std::endl;

                if (debug) {
                    std::cout << "Candidates for next tile: ";
                    for (auto& c : checksum_to_tile[hash(checksum_to_find)]) {
                        std::cout << c->getId() << ",";
                    }
                    std::cout << std::endl;
                }

                auto candidate = checksum_to_tile[hash(checksum_to_find)][0];
                while (hash(candidate->getSidesChecksum()[0]) != hash(tile_above->getSidesChecksum()[2])) {
                    candidate->rotate();
                }
                if (candidate->getSidesChecksum()[0] != tile_above->getSidesChecksum()[2]) {
                    if (debug) std::cout << "need to flip horizontally " << candidate->getId() << std::endl;
                    candidate->flip_horizontally();
                }

                candidate->set_used();
                f.set_at(i, y, candidate);
                remove_tile_from_checksum_map(candidate, checksum_to_tile);
            }
            if (debug) std::cout << std::endl;
        }

        auto im = f.get_image();

        if (debug) {
            std::cout << "Tiles in frame" << std::endl;
            f.dump_ids();
            std::cout << std::endl;

            std::cout << "Image with borders" << std::endl;
            f.dump_ascii();
            std::cout << std::endl;

            std::cout << "Image without borders" << std::endl;
            im.dump();
            std::cout << std::endl;
        }

        std::tuple<int, int> max_nessies{0, 0};
        for (int i = 0; i < 4; i++) {
            if (i > 0) im.rotate();
            auto r = im.count_nessies();
            if (std::get<0>(r) > std::get<0>(max_nessies)) {
                max_nessies = r;
            }
        }

        if (debug) std::cout << "Flipping vertically" << std::endl;
        im.flip_vertically();
        for (int i = 0; i < 4; i++) {
            if (i > 0) im.rotate();
            auto r = im.count_nessies();
            if (std::get<0>(r) > std::get<0>(max_nessies)) {
                max_nessies = r;
            }
        }

        if (debug) std::cout << "Flipping horizontally" << std::endl;
        im.flip_horizontally();
        for (int i = 0; i < 4; i++) {
            if (i > 0) im.rotate();
            auto r = im.count_nessies();
            if (std::get<0>(r) > std::get<0>(max_nessies)) {
                max_nessies = r;
            }
        }

        // undo the effect of first vertical flip, only leaving horizontal flip enabled
        if (debug) std::cout << "Flipping vertically" << std::endl;
        im.flip_vertically();
        for (int i = 0; i < 4; i++) {
            if (i > 0) im.rotate();
            auto r = im.count_nessies();
            if (std::get<0>(r) > std::get<0>(max_nessies)) {
                max_nessies = r;
            }
        }

        if (debug)
            std::cout << "Nessies found: " << std::get<0>(max_nessies)
                      << ", roughness: " << std::get<1>(max_nessies)
                      << std::endl;

        out << std::get<1>(max_nessies) << std::endl;
    }

    void register_tests() override {
        add_test(aoc::test{
                .name = "First",
                .input = "Tile 2311:\n"
                         "..##.#..#.\n"
                         "##..#.....\n"
                         "#...##..#.\n"
                         "####.#...#\n"
                         "##.##.###.\n"
                         "##...#.###\n"
                         ".#.#.#..##\n"
                         "..#....#..\n"
                         "###...#.#.\n"
                         "..###..###\n"
                         "\n"
                         "Tile 1951:\n"
                         "#.##...##.\n"
                         "#.####...#\n"
                         ".....#..##\n"
                         "#...######\n"
                         ".##.#....#\n"
                         ".###.#####\n"
                         "###.##.##.\n"
                         ".###....#.\n"
                         "..#.#..#.#\n"
                         "#...##.#..\n"
                         "\n"
                         "Tile 1171:\n"
                         "####...##.\n"
                         "#..##.#..#\n"
                         "##.#..#.#.\n"
                         ".###.####.\n"
                         "..###.####\n"
                         ".##....##.\n"
                         ".#...####.\n"
                         "#.##.####.\n"
                         "####..#...\n"
                         ".....##...\n"
                         "\n"
                         "Tile 1427:\n"
                         "###.##.#..\n"
                         ".#..#.##..\n"
                         ".#.##.#..#\n"
                         "#.#.#.##.#\n"
                         "....#...##\n"
                         "...##..##.\n"
                         "...#.#####\n"
                         ".#.####.#.\n"
                         "..#..###.#\n"
                         "..##.#..#.\n"
                         "\n"
                         "Tile 1489:\n"
                         "##.#.#....\n"
                         "..##...#..\n"
                         ".##..##...\n"
                         "..#...#...\n"
                         "#####...#.\n"
                         "#..#.#.#.#\n"
                         "...#.#.#..\n"
                         "##.#...##.\n"
                         "..##.##.##\n"
                         "###.##.#..\n"
                         "\n"
                         "Tile 2473:\n"
                         "#....####.\n"
                         "#..#.##...\n"
                         "#.##..#...\n"
                         "######.#.#\n"
                         ".#...#.#.#\n"
                         ".#########\n"
                         ".###.#..#.\n"
                         "########.#\n"
                         "##...##.#.\n"
                         "..###.#.#.\n"
                         "\n"
                         "Tile 2971:\n"
                         "..#.#....#\n"
                         "#...###...\n"
                         "#.#.###...\n"
                         "##.##..#..\n"
                         ".#####..##\n"
                         ".#..####.#\n"
                         "#..#.#..#.\n"
                         "..####.###\n"
                         "..#.#.###.\n"
                         "...#.#.#.#\n"
                         "\n"
                         "Tile 2729:\n"
                         "...#.#.#.#\n"
                         "####.#....\n"
                         "..#.#.....\n"
                         "....#..#.#\n"
                         ".##..##.#.\n"
                         ".#.####...\n"
                         "####.#.#..\n"
                         "##.####...\n"
                         "##..#.##..\n"
                         "#.##...##.\n"
                         "\n"
                         "Tile 3079:\n"
                         "#.#.#####.\n"
                         ".#..######\n"
                         "..#.......\n"
                         "######....\n"
                         "####.#..#.\n"
                         ".#...#.##.\n"
                         "#.#####.##\n"
                         "..#.###...\n"
                         "..#.......\n"
                         "..#.###...",
                .expected_output = "20899048083289\n273"
        });
    }
};

int main(int argc, char** argv) {
    day20 solution{};
    return solution.execute(argc, argv);
}

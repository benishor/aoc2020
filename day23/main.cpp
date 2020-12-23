#include "solution.h"
#include <list>
#include <utility>
#include <numeric>

const bool debug = false;

class day23 : public aoc::solution {
protected:

    static std::list<int> play_game(std::list<int> data, unsigned int nr_moves) {
        if (debug) fmt::print("{}\n", fmt::join(data, ","));

        auto min_cup = *std::min_element(data.begin(), data.end());
        auto max_cup = *std::max_element(data.begin(), data.end());

        std::bitset<1'000'001> picked_up_cup_flags{0};

        int current_cup_label{*data.begin()};
        std::list<int> picked_up_items{};

        for (unsigned int round = 0; round < nr_moves; round++) {
            if ((round % 10000) == 0) {
//                std::cout << "Done: " << (round * 100 / nr_moves) << "%       \r";
                std::cout << "Round: " << round << "%       \r";
                std::cout.flush();
            }
            if (debug) fmt::print("-- move {} --\n", round + 1);
            if (debug) fmt::print("cups: {}\n", fmt::join(data, ","));

            auto it = std::find(data.begin(), data.end(), current_cup_label);
            picked_up_items.clear();
            it++;
            for (int i = 0; i < 3; i++) {
                if (it == data.end()) it = data.begin();
                picked_up_items.push_back(*it);
                picked_up_cup_flags.set(*it, true);
                it = data.erase(it);
            }

            if (debug) fmt::print("pick up: {}\n", fmt::join(picked_up_items, ","));

            // select destination cup
            int destination_cup_label = current_cup_label;
            do {
                destination_cup_label--;
                if (destination_cup_label < min_cup) destination_cup_label = max_cup;
            } while (picked_up_cup_flags[destination_cup_label]);
            if (debug) fmt::print("destination: {}\n", destination_cup_label);

            it = std::find(data.begin(), data.end(), destination_cup_label);
            it++;
            data.insert(it, picked_up_items.begin(), picked_up_items.end());
            picked_up_cup_flags = 0;

            it = std::find(data.begin(), data.end(), current_cup_label);
            it++;
            if (it == data.end()) it = data.begin();

            current_cup_label = *it;
            if (debug) std::cout << std::endl;
        }

        if (debug) fmt::print("cups: {}\n", fmt::join(data, ","));

        while (*data.begin() != 1) {
            auto next = data.begin();
            std::advance(next, 1);
            std::rotate(data.begin(), next, data.end());
        }
        data.pop_front();
        return data;
    }

    static std::string part_1(std::vector<int> data) {
        play_game_2(data, 100);
        while (*data.begin() != 1) {
            auto next = data.begin();
            std::advance(next, 1);
            std::rotate(data.begin(), next, data.end());
        }
        data.erase(data.begin());

        return fmt::format("{}", fmt::join(data, ""));
    }

    static std::tuple<int, int> play_game_2(std::vector<int>& data, unsigned int rounds) {
        auto current_cup_position = 0;
        auto current_cup = data[current_cup_position];
        std::bitset<1'000'0001> picked_up_cup_flags{0};
        std::array<int, 3> picked_up_items = {0};

        unsigned int first_pickup_position{0};
        unsigned int next_current_cup{0};
        unsigned int nr_cups = data.size();

        std::cout << std::endl;
        for (unsigned int round = 0; round < rounds; round++) {

            if ((round % 10000) == 0) {
                std::cout << "Round: " << (round * 100 / rounds) << "%       \r";
                std::cout.flush();
            }

            if (debug) fmt::print("-- move {} --\n", round + 1);
            if (debug) fmt::print("cups: {}\n", fmt::join(data, ","));

            // pick up the items
            for (int i = 0; i < 4; i++) {
                auto pickup_position = (current_cup_position + i + 1) % nr_cups;
                auto value = data[pickup_position];
                if (i == 0) {
                    first_pickup_position = pickup_position;
                }

                if (i < 3) {
                    picked_up_items[i] = value;
                    picked_up_cup_flags[value] = true;
                } else {
                    next_current_cup = value;
                }
            }
            if (debug) fmt::print("pick up: {}\n", fmt::join(picked_up_items, ","));

            // determine destination
            auto destination_value = current_cup;
            do {
                destination_value--;
                if (destination_value < 1) destination_value = nr_cups;
            } while (picked_up_cup_flags[destination_value]);
            if (debug) fmt::print("destination: {}\n\n", destination_value);

            // remove the picked up items
            if (first_pickup_position <= (nr_cups - 3)) {
                auto start = data.begin() + first_pickup_position;
                data.erase(start, start + 3);
//                memmove(data_ptr + first_pickup_position,
//                        data_ptr + first_pickup_position + 3,
//                        1'000'000 - first_pickup_position - 3);
            } else {
                unsigned int how_many_to_remove_from_start = ((first_pickup_position + 3) % nr_cups);
                data.erase(data.begin(), data.begin() + how_many_to_remove_from_start);
                unsigned int how_many_to_remove_from_end = 3 - how_many_to_remove_from_start;
                for (unsigned int i = 0; i < how_many_to_remove_from_end; i++) {
                    data.pop_back();
                }
            }

            // insert the picked up items
            auto insert_pos_it = std::find(data.begin(), data.end(), destination_value);
            data.insert(insert_pos_it + 1, std::begin(picked_up_items), std::end(picked_up_items));

            // next iteration
            current_cup = next_current_cup;
            current_cup_position = std::find(data.begin(), data.end(), current_cup) - data.begin();
            picked_up_cup_flags = 0;
        }

        // find 1 and return next two clockwise elements
        auto pos_of_cup_one = std::find(data.begin(), data.end(), 1) - data.begin();
        return std::make_tuple(data[(pos_of_cup_one + 1) % nr_cups], data[(pos_of_cup_one + 2) % nr_cups]);
    }

    std::string part_2(const std::vector<int>& data) {
        // prepare data
        std::vector<int> data2(1'000'000, 0);
        std::iota(data2.begin(), data2.end(), 1);

        for (size_t i = 0; i < data.size(); i++) {
            data2[i] = data[i];
        }

        auto result = play_game_2(data2, 10'000'000);
        return fmt::format("{} {}", std::get<0>(result), std::get<1>(result));
    }

    void run(std::istream& in, std::ostream& out) override {
        std::vector<int> data{};
        if (std::string line; std::getline(in, line)) {
            for (auto c : aoc::trim(line)) {
                data.push_back(c - '0');
            }
        }
        out << part_1(data) << std::endl;
        out << part_2(data) << std::endl;
    }

    void register_tests() override {
//        add_test(aoc::test{
//                .name = "First",
//                .input = "389125467",
//                .expected_output = "67384529"
//        });
        add_test(aoc::test{
                .name = "Mine",
                .input = "942387615",
                .expected_output = "36542897"
        });
    }
};

int main(int argc, char** argv) {
    day23 solution{};
    return solution.execute(argc, argv);
}

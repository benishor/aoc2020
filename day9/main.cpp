#include "solution.h"
#include <deque>
#include <algorithm>


class day9 : public aoc::solution {
protected:

    static const int WindowSize = 25;
    using ElementType = long long;

    void run(std::istream& in, std::ostream& out) override {
        auto elements = aoc::elements_from_stream<ElementType>(in);

        auto invalid_number = part_1(elements);
        out << invalid_number << std::endl;

        auto encryption_weakness = part_2(elements, invalid_number);
        out << encryption_weakness << std::endl;
    }

    static ElementType part_1(const std::vector<ElementType>& elements) {
        std::deque<ElementType> window;
        for (const auto& e : elements) {
            if (window.size() < WindowSize) {
                window.push_back(e);
            } else {
                if (is_number_in_window_as_sum(e, window)) {
                    window.pop_front();
                    window.push_back(e);
                } else {
                    return e;
                }
            }
        }
        std::cerr << "should not get here" << std::endl;
        return -1;
    }

    static bool is_number_in_window_as_sum(const ElementType& number, const std::deque<long long>& window) {
        for (size_t i = 0; i < window.size() - 1; i++) {
            for (size_t j = i + 1; j < window.size(); j++) {
                if ((window.at(i) + window.at(j)) == number) return true;
            }
        }
        return false;
    }

    static ElementType part_2(const std::vector<ElementType>& elements, ElementType invalid_number) {
        ElementType running_sum = 0L;

        auto start = elements.begin();
        auto end = elements.begin();
        while (end != elements.end()) {
            running_sum += *end++;
            while (running_sum > invalid_number && start < end) {
                running_sum -= *start++;
            }
            if (running_sum == invalid_number) {
                auto min_value = std::min_element(start, end);
                auto max_value = std::max_element(start, end);
                auto sum = *min_value + *max_value;
                return sum;
            }
        }

        std::cerr << "Should not get here";
        return -1;
    }

    void register_tests() override {
        // WindowSize must be 5 in order for the test to work
        add_test(aoc::test{
                .name = "First",
                .input = "35\n"
                         "20\n"
                         "15\n"
                         "25\n"
                         "47\n"
                         "40\n"
                         "62\n"
                         "55\n"
                         "65\n"
                         "95\n"
                         "102\n"
                         "117\n"
                         "150\n"
                         "182\n"
                         "127\n"
                         "219\n"
                         "299\n"
                         "277\n"
                         "309\n"
                         "576",
                .expected_output = "127\n62"
        });
    }
};

int main(int argc, char** argv) {
    day9 solution;
    return solution.execute(argc, argv);
}
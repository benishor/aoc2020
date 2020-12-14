#include "solution.h"

using element_type = __uint128_t;

std::ostream& operator<<(std::ostream& out, __uint128_t n) {
    if (n == 0) {
        out << "0";
        return out;
    }

    char str[40] = {0};
    char* s = str + sizeof(str) - 1;
    while (n != 0) {
        if (s == str) return out; // never happens
        *--s = "0123456789"[n % 10]; // save last digit
        n /= 10;                     // drop it
    }
    out << s;
    return out;
}

const static bool debug = false;

class day14 : public aoc::solution {
protected:

    std::regex mask_re{"^mask\\s*=\\s*([X01]+)$"};
    std::regex poke_re{R"(^mem\[([0-9]+)\]\s*=\s*([0-9]+)$)"};

    element_type part_1(const std::vector<std::string>& lines) {
        std::map<element_type, element_type> memory;
        element_type mask_to_set{0};
        element_type mask_value{0};
        for (const auto& line : lines) {
            std::smatch sm{};
            if (std::regex_match(line, sm, mask_re)) {
                auto mask = sm.str(1);
                mask_to_set = 0;
                mask_value = 0;
                if (debug) std::cout << "Mask as string:\t" << mask << std::endl;
                for (char& it : mask) {
                    mask_to_set = mask_to_set << 1;
                    if (it == 'X') {
                        mask_to_set |= 1;
                    }
                    if (debug) std::cout << "Mask to set:\t" << std::bitset<64>(mask_to_set) << std::endl;

                    mask_value <<= 1;
                    mask_value |= (it == '1');
                }
                if (debug) {
                    std::cout << "Mask to set:\t" << std::bitset<64>(mask_to_set) << std::endl;
                    std::cout << "Mask value:\t\t" << std::bitset<64>(mask_value) << std::endl;
                }
            } else if (std::regex_match(line, sm, poke_re)) {
                element_type offset = std::stoi(sm.str(1));
                element_type value = std::stoi(sm.str(2));
                element_type masked_value = (value & mask_to_set) | mask_value;
                memory[offset] = masked_value;
            }
        }

        element_type result{0};
        for (auto& kv : memory) {
            result += kv.second;
        }
        return result;
    }

    element_type part_2(const std::vector<std::string>& lines) {
        std::map<element_type, element_type> memory;
        std::string mask;

        for (const auto& line : lines) {
            if (debug) std::cout << line << std::endl;
            std::smatch sm{};
            if (std::regex_match(line, sm, mask_re)) {
                mask = sm.str(1);
                if (debug) std::cout << "Mask: " << mask << std::endl;
            } else if (std::regex_match(line, sm, poke_re)) {
                element_type offset = std::stoi(sm.str(1));
                element_type value = std::stoi(sm.str(2));
                element_type modified_offset = offset;

                if (debug) {
                    std::cout << "---\nAttempting to set offset " << offset << " to value " << value << std::endl;
                    std::cout << "Mask:\t\t" << mask << std::endl;
                    std::cout << "Offset:\t\t" << std::bitset<36>(offset) << std::endl;
                }

                std::vector<int> floating_bits;
                auto it = mask.rbegin();
                for (int i = 0; i < 36; i++) {
                    if (*it == 'X') {
                        floating_bits.push_back(i);
                    } else if (*it == '1') {
                        modified_offset |= (1UL << i);
                    }
                    it++;
                }

                if (debug) {
                    std::cout << "Modified:\t" << std::bitset<36>(modified_offset) << std::endl;
                    fmt::print("Floating bits: {}\n", fmt::join(floating_bits, ","));
                }

                for (size_t i = 0; i < (1U << floating_bits.size()); i++) {
                    auto current_offset = modified_offset;
                    for (size_t j = 0; j < floating_bits.size(); j++) {
                        if (i & (1 << j)) {
                            current_offset |= 1UL << floating_bits[j];
                        } else {
                            current_offset &= ~(1UL << floating_bits[j]);
                        }
                    }
                    if (debug)
                        std::cout << "Setting (" << std::bitset<36>(current_offset) << ") " << current_offset << " to "
                                  << value << std::endl;
                    memory[current_offset] = value;
                }
            }
        }

        __uint128_t result{0};
        for (auto& kv : memory) {
            result += kv.second;
        }
        return result;
    }

    void run(std::istream& in, std::ostream& out) override {
        std::vector<std::string> lines;
        for (std::string line; std::getline(in, line);) {
            lines.push_back(aoc::trim(line));
        }
        out << part_1(lines) << std::endl;
        out << part_2(lines) << std::endl;
    }

    void register_tests() override {
        add_test(aoc::test{
                .name = "First",
                .input = "mask = XXXXXXXXXXXXXXXXXXXXXXXXXXXXX1XXXX0X\n"
                         "mem[8] = 11\n"
                         "mem[7] = 101\n"
                         "mem[8] = 0",
                .expected_output = "165\n404"
        });
        add_test(aoc::test{
                .name = "Second",
                .input = "mask = 000000000000000000000000000000X1001X\n"
                         "mem[42] = 100\n"
                         "mask = 00000000000000000000000000000000X0XX\n"
                         "mem[26] = 1",
                .expected_output = "51\n208"
        });
    }
};

int main(int argc, char** argv) {
    day14 solution{};
    return solution.execute(argc, argv);
}

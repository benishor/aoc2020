#include <iostream>
#include "solution.h"
#include <algorithm>

using namespace std;

struct password_info {
    int low;
    int high;
    char letter;
    string text;
};

template<>
struct fmt::formatter<password_info> {
    static constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const password_info& p, FormatContext& ctx) {
        return format_to(
                ctx.out(),
                "{{.low: {0}, .high: {1}, .letter: {2}, text: \"{3}\"}}",
                p.low, p.high, p.letter, p.text);
    }
};

class day2 : public aoc::solution {
public:
    void run(std::istream& in, std::ostream& out) override {
        std::vector<password_info> passwords = read_passwords(in);
        out << count_if(passwords.begin(), passwords.end(), is_password_valid_1) << std::endl;
        out << count_if(passwords.begin(), passwords.end(), is_password_valid_2) << std::endl;
    }

    static vector<password_info> read_passwords(istream& in) {
        static const regex rePassword("^(\\d+)-(\\d+) ([a-zA-Z]): (.*)$");
        vector<password_info> passwords;
        for (string line; getline(in, line);) {
            smatch m{};
            if (regex_match(line, m, rePassword))
                passwords.push_back(password_info{
                        .low = stoi(m[1].str()),
                        .high = stoi(m[2].str()),
                        .letter = m[3].str().at(0),
                        .text = m[4].str()});
            else
                fmt::print(stderr, fmt::format("Failed to parse [{}]", line));
        }
        return passwords;
    }

    static bool is_password_valid_1(const password_info& p) {
        auto occurrences = std::count_if(p.text.begin(), p.text.end(), [&](auto a) {
            return a == p.letter;
        });
        return occurrences >= p.low && occurrences <= p.high;
    }

    static bool is_password_valid_2(const password_info& p) {
        if (p.low >= p.text.size() || p.high > p.text.size()) {
            return false;
        }
        auto low = p.low - 1;
        auto high = p.high - 1;
        return p.text.at(low) == p.letter && p.text.at(high) != p.letter ||
               p.text.at(high) == p.letter && p.text.at(low) != p.letter;
    }

    void register_tests() override {
        add_test({
                         .name = "First",
                         .input = "1-3 a: abcde\n"
                                  "1-3 b: cdefg\n"
                                  "2-9 c: ccccccccc",
                         .expectedOutput = "2\n1"
                 });
    }
};

int main(int argc, char** argv) {
    day2 solution;
    return solution.execute(argc, argv);
}

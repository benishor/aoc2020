#include <numeric>
#include "solution.h"

class day19 : public aoc::solution {
protected:
    void run(std::istream& in, std::ostream& out) override {

        std::vector<std::string> patterns{};
        std::map<long, std::string> rules;

        for (std::string line; std::getline(in, line);) {
            line = aoc::trim(line);
            if (!line.empty()) {
                static std::regex re_rule{R"(^(\d+):(.*)$)"};
                std::smatch sm;
                if (std::regex_match(line, sm, re_rule)) {
                    auto rule_id = std::stol(aoc::trim(sm.str(1)));
                    auto rule_definition = aoc::trim(sm.str(2));
                    rules[rule_id] = rule_definition;
                } else {
                    patterns.push_back(line);
                }
            }
        }


        auto rule_regexp = "^" + build_regexp(rules[0], rules) + "$";
        std::regex re_root{rule_regexp};
        int part_1 = std::accumulate(patterns.begin(),
                                     patterns.end(),
                                     0,
                                     [&](auto x, const std::string& pattern) {
                                         return x + std::regex_match(pattern, re_root);
                                     });
        out << part_1 << std::endl;

        // part 2 hack
        rules[8] = "42+";
        rules[11] = "42 31 | 42 9000 31";
        for (int i = 9000; i < 9004; i++) {
            rules[i] = "42 31 | 42 " + std::to_string((i + 1)) + " 31";
        }
        rules[9004] = "42 31";

        auto rule_regexp_2 = "^" + build_regexp(rules[0], rules) + "$";
        std::regex re_root_2{rule_regexp_2};
        int part_2 = std::accumulate(patterns.begin(),
                                     patterns.end(),
                                     0,
                                     [&](auto x, const std::string& pattern) {
                                         return x + std::regex_match(pattern, re_root_2);
                                     });
        out << part_2 << std::endl;
    }

    static std::string build_regexp(const std::string& description, const std::map<long, std::string>& rules) {
        auto or_pos = description.find_first_of('|');
        if (or_pos != std::string::npos) {
            auto a = aoc::trim(description.substr(0, or_pos));
            auto b = aoc::trim(description.substr(or_pos + 1));
            return "(?:" + build_regexp(a, rules) + "|" + build_regexp(b, rules) + ")";
        } else {
            auto pieces = aoc::split_string(description, " ");
            if (pieces.size() == 1) {
                auto p = pieces[0];
                if (p.size() == 3 && p[0] == '"' && p[2] == '"') {
                    std::stringstream ss;
                    ss << p[1];
                    return ss.str();
                } else {
                    auto id = std::stol(p);
                    auto result = build_regexp(rules.at(id), rules);
                    if (*p.rbegin() == '+') {
                        result = "(?:" + result + ")+";
                    }
                    return result;
                }
            } else {
                std::stringstream result;
                for (auto& p : pieces) {
                    result << build_regexp(p, rules);
                }
                return result.str();
            }
        }
    }

    void register_tests() override {
        add_test(aoc::test{
                .name = "First",
                .input = "0: 4 1 5\n"
                         "1: 2 3 | 3 2\n"
                         "2: 4 4 | 5 5\n"
                         "3: 4 5 | 5 4\n"
                         "4: \"a\"\n"
                         "5: \"b\"\n"
                         "\n"
                         "ababbb\n"
                         "bababa\n"
                         "abbbab\n"
                         "aaabbb\n"
                         "aaaabbb",
                .expected_output = "2\n2"
        });
        add_test(aoc::test{
                .name = "Second",
                .input = "42: 9 14 | 10 1\n"
                         "9: 14 27 | 1 26\n"
                         "10: 23 14 | 28 1\n"
                         "1: \"a\"\n"
                         "11: 42 31\n"
                         "5: 1 14 | 15 1\n"
                         "19: 14 1 | 14 14\n"
                         "12: 24 14 | 19 1\n"
                         "16: 15 1 | 14 14\n"
                         "31: 14 17 | 1 13\n"
                         "6: 14 14 | 1 14\n"
                         "2: 1 24 | 14 4\n"
                         "0: 8 11\n"
                         "13: 14 3 | 1 12\n"
                         "15: 1 | 14\n"
                         "17: 14 2 | 1 7\n"
                         "23: 25 1 | 22 14\n"
                         "28: 16 1\n"
                         "4: 1 1\n"
                         "20: 14 14 | 1 15\n"
                         "3: 5 14 | 16 1\n"
                         "27: 1 6 | 14 18\n"
                         "14: \"b\"\n"
                         "21: 14 1 | 1 14\n"
                         "25: 1 1 | 1 14\n"
                         "22: 14 14\n"
                         "8: 42\n"
                         "26: 14 22 | 1 20\n"
                         "18: 15 15\n"
                         "7: 14 5 | 1 21\n"
                         "24: 14 1\n"
                         "\n"
                         "abbbbbabbbaaaababbaabbbbabababbbabbbbbbabaaaa\n"
                         "bbabbbbaabaabba\n"
                         "babbbbaabbbbbabbbbbbaabaaabaaa\n"
                         "aaabbbbbbaaaabaababaabababbabaaabbababababaaa\n"
                         "bbbbbbbaaaabbbbaaabbabaaa\n"
                         "bbbababbbbaaaaaaaabbababaaababaabab\n"
                         "ababaaaaaabaaab\n"
                         "ababaaaaabbbaba\n"
                         "baabbaaaabbaaaababbaababb\n"
                         "abbbbabbbbaaaababbbbbbaaaababb\n"
                         "aaaaabbaabaaaaababaa\n"
                         "aaaabbaaaabbaaa\n"
                         "aaaabbaabbaaaaaaabbbabbbaaabbaabaaa\n"
                         "babaaabbbaaabaababbaabababaaab\n"
                         "aabbbbbaabbbaaaaaabbbbbababaaaaabbaaabba",
                .expected_output = "3\n12"
        });
    }
};

int main(int argc, char** argv) {
    day19 solution{};
    return solution.execute(argc, argv);
}

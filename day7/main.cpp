#include <iostream>
#include "solution.h"
#include <algorithm>
#include <functional>


struct bag {
    std::string color;
    std::vector<std::tuple<std::string, int>> contents;
};

std::map<std::string, std::shared_ptr<bag>> bags;


class day7 : public aoc::solution {
public:

    bool bag_recursively_contains(const std::string& color, const std::string& desired_color) {
        if (color == desired_color) return true;
        auto b = bags[color];
        if (b->contents.empty()) return false;

        for (auto& kv: b->contents) {
            if (bag_recursively_contains(std::get<std::string>(kv), desired_color)) {
                return true;
            }
        }

        return false;
    }

    int count_bags_in(const std::string& color) {
        if (bags[color]->contents.empty()) return 0;

        auto result = 0;
        for (auto& c : bags[color]->contents) {
            result += std::get<int>(c);
            result += std::get<int>(c) * count_bags_in(std::get<std::string>(c));
        }

        return result;
    }

    void run(std::istream& in, std::ostream& out) override {
        static const std::regex re("^(.*) bags contain (.*).$");
        for (std::string line; std::getline(in, line);) {
            std::smatch m{};
            if (regex_match(line, m, re)) {
                auto b = std::make_shared<bag>();
                b->color = m[1].str();
                bags[b->color] = b;

                auto contents = m[2].str();
                if (contents != "no other bags") {
                    std::stringstream ss{contents};
                    for (std::string item; std::getline(ss, item, ',');) {
                        item = aoc::trim(item);
                        static const std::regex re2("^(\\d+) (.*) bags?$");
                        std::smatch m2{};
                        if (regex_match(item, m2, re2)) {
                            b->contents.emplace_back(m2[2].str(), std::stoi(m2[1].str()));
                        }
                    }
                }
            }
        }

//        for (auto& kv : bags) {
//            std::cout << kv.first << std::endl;
//            for (auto c : kv.second->contents) {
//                std::cout << "\t" << std::get<int>(c) << " x " << std::get<std::string>(c) << std::endl;
//            }
//        }

        int part_1 = -1; // do not count ourselves
        for (auto& kv : bags) {
            if (bag_recursively_contains(kv.first, "shiny gold")) {
                part_1++;
            }
        }
        out << part_1 << std::endl;

        int part_2 = count_bags_in("shiny gold");
        out << part_2 << std::endl;
    }

protected:
    void register_tests() override {
        add_test(aoc::test{
                .name = "First",
                .input = "light red bags contain 1 bright white bag, 2 muted yellow bags.\n"
                         "dark orange bags contain 3 bright white bags, 4 muted yellow bags.\n"
                         "bright white bags contain 1 shiny gold bag.\n"
                         "muted yellow bags contain 2 shiny gold bags, 9 faded blue bags.\n"
                         "shiny gold bags contain 1 dark olive bag, 2 vibrant plum bags.\n"
                         "dark olive bags contain 3 faded blue bags, 4 dotted black bags.\n"
                         "vibrant plum bags contain 5 faded blue bags, 6 dotted black bags.\n"
                         "faded blue bags contain no other bags.\n"
                         "dotted black bags contain no other bags.",
                .expected_output = "4\n32"
        });
    }
};

int main(int argc, char** argv) {
    day7 solution;
    return solution.execute(argc, argv);
}
#include "solution.h"


struct food {
    std::vector<std::string> ingredients;
    std::vector<std::string> alergents;
};

std::ostream& operator<<(std::ostream& out, const food& f) {
    out << "{"
        << fmt::format("ingredients: {}, alergents: {}", fmt::join(f.ingredients, ","), fmt::join(f.alergents, ","))
        << "}";
    return out;
}

std::vector<std::string> intersection(std::vector<std::string>& v1,
                                      std::vector<std::string>& v2) {
    std::vector<std::string> v3;

    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());

    std::set_intersection(v1.begin(), v1.end(),
                          v2.begin(), v2.end(),
                          back_inserter(v3));
    return v3;
}


class day21 : public aoc::solution {
protected:
    void run(std::istream& in, std::ostream& out) override {
        std::vector<food> foods{};

        for (std::string line; std::getline(in, line);) {
            static std::regex re_food{R"(^(.*)\(contains(.*)\)$)"};
            std::smatch sm;
            if (std::regex_search(line, sm, re_food)) {
                auto ingredients = aoc::split_string(aoc::trim(sm.str(1)), " ");
                auto alergents = aoc::split_string(aoc::trim(sm.str(2)), ",");
                for (auto& a : alergents) {
                    aoc::trim(a);
                }
                foods.push_back({ingredients, alergents});
            } else {
                fmt::print(stderr, "Failed to match line " + line);
            }
        }

        std::map<std::string, std::vector<std::vector<std::string>>> alergent_to_possible_ingredients{};
        for (auto& f : foods) {
            for (const auto& a : f.alergents) {
                alergent_to_possible_ingredients[a].push_back(f.ingredients);
            }
        }

        for (auto& kv : alergent_to_possible_ingredients) {
            auto alergent = kv.first;
            if (kv.second.size() > 1) {
                auto x = kv.second.at(0);
                for (size_t i = 1; i < kv.second.size(); i++) {
                    x = intersection(x, kv.second[i]);
                }
                kv.second.clear();
                kv.second.push_back(x);
            }
        }

        bool do_continue;
        std::map<std::string, std::string> ingredient_to_alergent{};
        do {
            do_continue = false;

            for (auto& kv : alergent_to_possible_ingredients) {
                if (kv.second.at(0).size() == 1) {
                    auto to_remove = kv.second.at(0).at(0);
                    if (ingredient_to_alergent.count(to_remove) == 0) {
                        ingredient_to_alergent[to_remove] = kv.first;
                    }
                }
            }

            for (auto& kv : alergent_to_possible_ingredients) {
                if (kv.second.at(0).size() > 1) {
                    auto it = kv.second[0].begin();
                    while (it != kv.second[0].end()) {
                        if (ingredient_to_alergent.count((*it))) {
                            it = kv.second[0].erase(it);
                            do_continue = true;
                        } else {
                            it++;
                        }
                    }
                }
            }
        } while (do_continue);


        unsigned long part_1{0};
        for (auto& f : foods) {
            for (auto& ingredient : f.ingredients) {
                if (!ingredient_to_alergent.count(ingredient)) {
                    part_1++;
                }
            }
        }
        out << part_1 << std::endl;

        // part 2
        std::map<std::string, std::string> allergent_to_ingredient{};
        for (auto& kv : ingredient_to_alergent) {
            allergent_to_ingredient[kv.second] = kv.first;
        }

        std::vector<std::string> canonical_dangerous_ingredients{};
        for (auto& kv : allergent_to_ingredient) {
            canonical_dangerous_ingredients.push_back(kv.second);
        }
        out << fmt::format("{}", fmt::join(canonical_dangerous_ingredients, ",")) << std::endl;
    }

    void register_tests()
    override {
        add_test(aoc::test{
                         .name = "First",
                         .input = "mxmxvkd kfcds sqjhc nhms (contains dairy, fish)\n"
                                  "trh fvjkl sbzzf mxmxvkd (contains dairy)\n"
                                  "sqjhc fvjkl (contains soy)\n"
                                  "sqjhc mxmxvkd sbzzf (contains fish)",
                         .expected_output = "5\nmxmxvkd,sqjhc,fvjkl"
                 }
        );
    }
};


int main(int argc, char** argv) {
    day21 solution{};
    return solution.execute(argc, argv);
}

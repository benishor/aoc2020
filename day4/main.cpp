#include <iostream>
#include "solution.h"
#include <algorithm>
#include <numeric>

class day4 : public aoc::solution {
public:
    void run(std::istream& in, std::ostream& out) override {
        std::vector<std::map<std::string, std::string>> documents;

        std::map<std::string, std::string> current_document;
        for (std::string line; std::getline(in, line);) {
            if (aoc::trim(line).empty()) {
                if (!current_document.empty()) {
                    documents.push_back(current_document);
                    current_document.clear();
                }
            } else {
                std::stringstream s1(line);
                std::string item1;
                while (std::getline(s1, item1, ' ')) {
                    auto colon_pos = item1.find_first_of(':');
                    auto key = item1.substr(0, colon_pos);
                    auto value = item1.substr(colon_pos + 1);
                    current_document[key] = value;
                }
            }
        }
        if (!current_document.empty()) {
            documents.push_back(current_document);
        }

//        for (auto &d : documents) {
//            std::cout << "Document:" << std::endl;
//            for (auto& [key, value] : d) {
//                fmt::print("{}: {}\n", key, value);
//            }
//        }

        auto valid_documents = 0;
        for (auto& d : documents) {
            if (are_fields_present(d)) {
                valid_documents++;
            }
        }
        out << valid_documents << std::endl;

        valid_documents = 0;
        for (auto& d : documents) {
            if (are_fields_present(d) && are_fields_valid(d)) {
                valid_documents++;
            }
        }
        out << valid_documents << std::endl;

    }

    static bool are_fields_present(const std::map<std::string, std::string>& document) {
        static std::string required_fields[] = {"byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid"};
        int fields_found = 0;
        for (auto& key : required_fields) {
            if (document.count(key)) {
                fields_found++;
            }
        }
        return fields_found == 7;
    }

    static bool is_valid_color(std::basic_string<char> color_string) {
        const std::regex re("^#[0-9a-fA-f]{6}$");
        std::smatch m{};
        return std::regex_match(color_string, m, re);
    }

    static bool is_valid_eye_color(std::basic_string<char> color_string) {
        static const std::string colors[] = {"amb", "blu", "brn", "gry", "grn", "hzl", "oth"};
        for (auto& c : colors) {
            if (color_string == c) return true;
        }
        return false;
    }

    static bool is_passport_id_valid(std::basic_string<char> pid) {
        const std::regex re("^[0-9]{9}$");
        std::smatch m{};
        return std::regex_match(pid, m, re);
    }

    static bool is_four_digit_number(const std::basic_string<char>& text) {
        const std::regex re("^[0-9]{4}$");
        std::smatch m{};
        return std::regex_match(text, m, re);
    }

    static bool are_fields_valid(const std::map<std::string, std::string>& document) {
        if (!is_four_digit_number(document.at("byr"))) return false;
        int byr = std::stoi(document.at("byr"));
        if (byr < 1920 || byr > 2002) return false;

        if (!is_four_digit_number(document.at("iyr"))) return false;
        int iyr = std::stoi(document.at("iyr"));
        if (iyr < 2010 || iyr > 2020) return false;

        if (!is_four_digit_number(document.at("eyr"))) return false;
        int eyr = std::stoi(document.at("eyr"));
        if (eyr < 2020 || eyr > 2030) return false;

        auto hgt_with_units = document.at("hgt");
        if (hgt_with_units.size() < 3) return false;
        auto units = hgt_with_units.substr(hgt_with_units.size() - 2);
        auto hgt = std::stoi(hgt_with_units.substr(0, hgt_with_units.size() - 2));
        if (units == "cm") {
            if (hgt < 150 || hgt > 193) return false;
        } else if (units == "in") {
            if (hgt < 59 || hgt > 76) return false;
        } else {
            return false;
        }

        auto hcl = document.at("hcl");
        if (!is_valid_color(hcl)) return false;

        auto ecl = document.at("ecl");
        if (!is_valid_eye_color(ecl)) return false;

        auto pid = document.at("pid");
        if (!is_passport_id_valid(pid)) return false;

        return true;
    }

    void register_tests() override {
        add_test({
                         .name = "First",
                         .input = "eyr:1972 cid:100\n"
                                  "hcl:#18171d ecl:amb hgt:170 pid:186cm iyr:2018 byr:1926\n"
                                  "\n"
                                  "iyr:2019\n"
                                  "hcl:#602927 eyr:1967 hgt:170cm\n"
                                  "ecl:grn pid:012533040 byr:1946\n"
                                  "\n"
                                  "hcl:dab227 iyr:2012\n"
                                  "ecl:brn hgt:182cm pid:021572410 eyr:2020 byr:1992 cid:277\n"
                                  "\n"
                                  "hgt:59cm ecl:zzz\n"
                                  "eyr:2038 hcl:74454a iyr:2023\n"
                                  "pid:3556412378 byr:2007",
                         .expectedOutput = "4\n0"
                 });
        add_test({
                         .name = "Second",
                         .input = "pid:087499704 hgt:74in ecl:grn iyr:2012 eyr:2030 byr:1980\n"
                                  "hcl:#623a2f\n"
                                  "\n"
                                  "eyr:2029 ecl:blu cid:129 byr:1989\n"
                                  "iyr:2014 pid:896056539 hcl:#a97842 hgt:165cm\n"
                                  "\n"
                                  "hcl:#888785\n"
                                  "hgt:164cm byr:2001 iyr:2015 cid:88\n"
                                  "pid:545766238 ecl:hzl\n"
                                  "eyr:2022\n"
                                  "\n"
                                  "iyr:2010 hgt:158cm hcl:#b6652a ecl:blu byr:1944 eyr:2021 pid:093154719",
                         .expectedOutput = "4\n4"
                 });
    }
};

int main(int argc, char** argv) {
    day4 solution;
    return solution.execute(argc, argv);
}
#include <iostream>
#include "solution.h"
#include <algorithm>

class day4 : public aoc::solution {
public:
    void run(std::istream& in, std::ostream& out) override {
        auto documents = read_documents(in);

        auto valid_documents_1 = 0, valid_documents_2 = 0;
        for (const auto& d : documents) {
            if (are_fields_present(d)) {
                valid_documents_1++;
                if (are_fields_valid(d)) {
                    valid_documents_2++;
                }
            }
        }
        out << valid_documents_1 << std::endl;
        out << valid_documents_2 << std::endl;
    }

    static std::vector<std::map<std::string, std::string>> read_documents(std::istream& in) {
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
        return documents;
    }

    static bool are_fields_present(const std::map<std::string, std::string>& document) {
        static std::string required_fields[] = {"byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid"};
        for (auto& key : required_fields) {
            if (!document.count(key)) {
                return false;
            }
        }
        return true;
    }

    static bool are_fields_valid(const std::map<std::string, std::string>& document) {
        return (is_valid_birth_year(document.at("byr")) &&
                is_valid_issue_year(document.at("iyr")) &&
                is_valid_expiration_year(document.at("eyr")) &&
                is_valid_height(document.at("hgt")) &&
                is_valid_hair_color(document.at("hcl")) &&
                is_valid_eye_color(document.at("ecl")) &&
                is_passport_id_valid(document.at("pid")));
    }

    static bool is_valid_birth_year(const std::string& text) {
        if (!is_four_digit_number(text)) return false;
        int eyr = std::stoi(text);
        return (eyr >= 1920 && eyr <= 2002);
    }

    static bool is_valid_issue_year(const std::string& text) {
        if (!is_four_digit_number(text)) return false;
        int eyr = std::stoi(text);
        return (eyr >= 2010 && eyr <= 2020);
    }

    static bool is_valid_expiration_year(const std::string& text) {
        if (!is_four_digit_number(text)) return false;
        int eyr = std::stoi(text);
        return (eyr >= 2020 && eyr <= 2030);
    }

    static bool is_valid_height(const std::string& text) {
        if (text.size() < 3) return false;
        auto units = text.substr(text.size() - 2);
        auto hgt = std::stoi(text.substr(0, text.size() - 2));
        if (units == "cm") {
            if (hgt < 150 || hgt > 193) return false;
        } else if (units == "in") {
            if (hgt < 59 || hgt > 76) return false;
        } else {
            return false;
        }
        return true;
    }

    static bool is_valid_hair_color(const std::string& text) {
        const static std::regex re("^#[0-9a-fA-f]{6}$");
        std::smatch m{};
        return std::regex_match(text, m, re);
    }

    static bool is_valid_eye_color(const std::string& text) {
        const static std::string colors[] = {"amb", "blu", "brn", "gry", "grn", "hzl", "oth"};
        return std::any_of(begin(colors), end(colors), [&](auto a) { return a == text; });
    }

    static bool is_passport_id_valid(const std::string& text) {
        const static std::regex re("^[0-9]{9}$");
        std::smatch m{};
        return std::regex_match(text, m, re);
    }

    static bool is_four_digit_number(const std::string& text) {
        const static std::regex re("^[0-9]{4}$");
        std::smatch m{};
        return std::regex_match(text, m, re);
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
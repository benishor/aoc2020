#include "solution.h"
#include <algorithm>

using element_type = long;
using range_type = std::tuple<element_type, element_type>;
struct field {
    std::string name;
    std::vector<range_type> valid_ranges;

    [[nodiscard]] bool is_value_valid(element_type value) const {
        for (const auto& range : valid_ranges) {
            if (std::get<0>(range) <= value && std::get<1>(range) >= value)
                return true;
        }
        return false;
    }
};

std::ostream& operator<<(std::ostream& out, const field& f) {
    out << "{name: " << f.name << ". valid_ranges: ";
    for (const auto& range : f.valid_ranges) {
        out << std::get<0>(range) << "-" << std::get<1>(range) << " ";
    }
    out << "}";
    return out;
}

class day16 : public aoc::solution {
protected:

    enum class ParserState {
        Fields,
        EndOfSection,
        YourTicket,
        NearbyTickets
    };

    void run(std::istream& in, std::ostream& out) override {
        std::vector<field> fields;
        std::vector<std::vector<element_type>> nearby_tickets;
        std::vector<element_type> your_ticket;

        element_type part_1 = 0;
        ParserState parser_state = ParserState::Fields;
        for (std::string line; std::getline(in, line);) {
            line = aoc::trim(line);
            if (line.empty()) {
                parser_state = ParserState::EndOfSection;
            } else if (line == "your ticket:") {
                parser_state = ParserState::YourTicket;
            } else if (line == "nearby tickets:") {
                parser_state = ParserState::NearbyTickets;
            } else {
                switch (parser_state) {
                    case ParserState::Fields: {
                        static std::regex re_field{R"(^([^:]+):\s*(\d+)\s*-\s*(\d+)\s*or\s*(\d+)\s*-\s*(\d+)$)"};
                        std::smatch sm;
                        if (std::regex_match(line, sm, re_field)) {
                            field f{.name = sm.str(1), .valid_ranges{}};
                            f.valid_ranges.emplace_back(std::stol(sm.str(2)), std::stol(sm.str(3)));
                            f.valid_ranges.emplace_back(std::stol(sm.str(4)), std::stol(sm.str(5)));
                            fields.push_back(f);
                        } else {
                            fmt::print(stderr, "Could not parse fields line {}", line);
                        }
                        break;
                    }
                    case ParserState::YourTicket: {
                        auto ss = std::stringstream{line};
                        your_ticket = aoc::csv_elements_from_stream<element_type>(ss);
                        break;
                    }
                    case ParserState::NearbyTickets: {
                        auto ss = std::stringstream{line};
                        auto field_values = aoc::csv_elements_from_stream<element_type>(ss);

                        bool all_fields_valid = true;
                        for (const auto& fv : field_values) {
                            bool field_is_valid = false;
                            for (const auto& f : fields) {
                                if (f.is_value_valid(fv)) {
                                    field_is_valid = true;
                                    break;
                                }
                            }
                            if (!field_is_valid) {
                                part_1 += fv;
                                all_fields_valid = false;
                            }
                        }
                        if (all_fields_valid) {
                            nearby_tickets.push_back(field_values);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
        }

        out << part_1 << std::endl;

        // ------------- part 2

        element_type part_2 = 1;

        // initial column to field candidates
        std::map<element_type, std::vector<element_type>> column_to_fields;
        auto nr_columns = nearby_tickets.at(0).size();
        for (size_t j = 0; j < nr_columns; j++) {
            for (size_t i = 0; i < fields.size(); i++) {
                bool is_valid_candidate = true;
                for (auto& fv : nearby_tickets) {
                    if (!fields[i].is_value_valid(fv.at(j))) {
                        is_valid_candidate = false;
                        break;
                    }
                }
                if (is_valid_candidate) {
                    column_to_fields[j].push_back(i);
                }
            }
        }

        std::map<element_type, element_type> column_to_field_mapping;
        bool do_continue;
        do {
            do_continue = false;
            for (auto& kv : column_to_fields) {
                auto column = kv.first;
                auto& candidate_fields = kv.second;
                if (candidate_fields.size() == 1) {
                    auto order = candidate_fields.at(0);
                    column_to_field_mapping[column] = order;
                }
            }

            for (auto& kv : column_to_fields) {
                auto& candidate_fields = kv.second;
                if (candidate_fields.size() > 1) {
                    for (auto& kv2 : column_to_field_mapping) {
                        auto& field2 = kv2.second;
                        auto it2 = std::find(kv.second.begin(), kv.second.end(), field2);
                        if (it2 != kv.second.end()) {
                            kv.second.erase(it2);
                            do_continue = true;
                        }
                    }
                }
            }
        } while (do_continue);

        for (auto& kv : column_to_fields) {
            auto column = kv.first;
            auto field_id = kv.second.at(0);
//            fmt::print("{} -> {}\n", fields[field_id].name, column);
            if (fields[field_id].name.substr(0, 9) == "departure") {
                part_2 *= your_ticket.at(column);
            }
        }
        out << part_2 << std::endl;
    }
};

int main(int argc, char** argv) {
    day16 solution{};
    return solution.execute(argc, argv);
}

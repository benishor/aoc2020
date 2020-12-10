#include "solution.h"
#include <deque>
#include <algorithm>
#include <queue>

class dag {
public:
    
    void add_edge(int from_node, int to_node) {
        edges[from_node].push_back(to_node);
    }

    long count_paths_from_to_naive(int start, int end) {
        long path_count = 0;
        count_paths(start, end, path_count);
        return path_count;
    }

    void count_paths(int start, int end, long& path_count) {
        if (start == end) {
            path_count++;
            return;
        }
        for (const auto& node : edges.at(start)) {
            count_paths(node, end, path_count);
        }
    }

    long count_paths_from_to_optimized(int start, int end) {
        std::map<int, long> possible_paths;
        possible_paths[end] = 1;
        auto it = edges.rbegin();
        while (it != edges.rend()) {
            auto source = it->first;
            for (const auto& dest : it->second) {
                possible_paths[source] += possible_paths[dest];
            }
            it++;
        }

        return possible_paths[start];
    }

    std::map<int, std::vector<int>> edges;
};

class day10 : public aoc::solution {
protected:

    void run(std::istream& in, std::ostream& out) override {
        auto sorted_elements = aoc::elements_from_stream<int>(in);
        std::sort(sorted_elements.begin(), sorted_elements.end());

        out << part_1(sorted_elements) << std::endl;
        out << part_2(sorted_elements) << std::endl;
    }

    static long part_1(const std::vector<int>& sorted_elements) {
        std::vector<int> differences;
        auto value = 0;
        for (const auto& e : sorted_elements) {
            differences.push_back(e - value);
            value = e;
        }
        differences.push_back(3); // device is always 3 jolts higher

        auto diff_1_jolt = std::count_if(differences.begin(), differences.end(), [](auto x) { return x == 1; });
        auto diff_3_jolt = std::count_if(differences.begin(), differences.end(), [](auto x) { return x == 3; });
        return diff_1_jolt * diff_3_jolt;
    }

    static long part_2(const std::vector<int>& sorted_elements) {
        auto g = graph_from_elements(sorted_elements);
        auto device_jolts = *sorted_elements.rbegin() + 3;
        return g.count_paths_from_to_optimized(0, device_jolts);
    }

    static dag graph_from_elements(const std::vector<int>& elements) {
        dag g{};

        std::vector<int> to_be_inspected{};
        to_be_inspected.push_back(0);

        std::map<int, bool> visited_nodes{};
        while (!to_be_inspected.empty()) {
            std::map<int, bool> new_to_be_inspected{};
            for (const auto& start : to_be_inspected) {
                if (visited_nodes.count(start)) continue;
                visited_nodes[start] = true;
                for (const auto& e : elements) {
                    auto diff = e - start;
                    if (diff > 0 && diff < 4) {
                        g.add_edge(start, e);
                        new_to_be_inspected[e] = true;
                    }
                }
            }
            to_be_inspected.clear();
            for (auto& kv : new_to_be_inspected) {
                to_be_inspected.push_back(kv.first);
            }
        }
        // add last adapter to device edge
        auto device_jolts = *elements.rbegin() + 3;
        g.add_edge(*elements.rbegin(), device_jolts);

        return g;
    }

    void register_tests() override {
        add_test(aoc::test{
                .name = "First",
                .input = "16\n"
                         "10\n"
                         "15\n"
                         "5\n"
                         "1\n"
                         "11\n"
                         "7\n"
                         "19\n"
                         "6\n"
                         "12\n"
                         "4",
                .expected_output = "35\n8"
        });

        add_test(aoc::test{
                .name = "Second",
                .input = "28\n"
                         "33\n"
                         "18\n"
                         "42\n"
                         "31\n"
                         "14\n"
                         "46\n"
                         "20\n"
                         "48\n"
                         "47\n"
                         "24\n"
                         "23\n"
                         "49\n"
                         "45\n"
                         "19\n"
                         "38\n"
                         "39\n"
                         "11\n"
                         "1\n"
                         "32\n"
                         "25\n"
                         "35\n"
                         "8\n"
                         "17\n"
                         "7\n"
                         "9\n"
                         "4\n"
                         "2\n"
                         "34\n"
                         "10\n"
                         "3",
                .expected_output = "220\n19208"
        });
    }
};

int main(int argc, char** argv) {
    day10 solution;
    return solution.execute(argc, argv);
}
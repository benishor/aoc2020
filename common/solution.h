#ifndef AOC_SOLUTION_H
#define AOC_SOLUTION_H

#include <fmt/format.h>
#include <fstream>
#include <regex>
#include <gtest/gtest.h>

namespace aoc {
    struct test {
        std::string name;
        std::string input;
        std::string expected_output;
    };

    class solution {
    public:
        int execute(int argc, char** argv);

    protected:
        virtual void run(std::istream& in, std::ostream& out) = 0;

        virtual void register_tests() {};
        void add_test(const test& test);

    private:
        std::vector<test> tests{};

        bool run_test(const test& test);
        bool run_tests();
        int delegate_execution(int argc, char** argv);
    };

    int solution::execute(int argc, char** argv) {
        register_tests();
        return delegate_execution(argc, argv);
    }

    bool solution::run_test(const test& test) {
        fmt::print(stderr, "Running test {} ... ", test.name);
        std::stringstream out{""};
        std::stringstream in{test.input};

        try {
            run(in, out);

            // right trim
            std::string actualOutput = std::regex_replace(out.str(), std::regex("\\s+$"), std::string(""));
            if (actualOutput != test.expected_output) {
//            fmt::print(stderr, "FAILED\n\tinput: [{0}]\n\texpected: [{1}]\n\t but got: [{2}]\n",
//                       test.input, test.expectedOutput, actualOutput);
                fmt::print(stderr, "FAILED\n\texpected: [{1}]\n\t but got: [{2}]\n",
                           test.input, test.expected_output, actualOutput);
                return false;
            }
            fmt::print(stderr, "OK\n");
        } catch (std::exception& e) {
            fmt::print(stderr, "FAILED\n\tCause: {0}\n", e.what());
            return false;
        } catch (...) {
            fmt::print(stderr, "FAILED\n\tCause: uncaught exception\n");
            return false;
        }

        return true;
    }

    void solution::add_test(const test& test) {
        tests.push_back(test);
    }

    int solution::delegate_execution(int argc, char** argv) {
        if (argc > 1 && !strcmp(argv[1], "--test")) {
            int exitCode = 0;
            if (!tests.empty()) {
                exitCode = run_tests() ? 0 : 1;
            }
            testing::InitGoogleTest(&argc, argv);
            return exitCode | RUN_ALL_TESTS();
        } else if (argc > 1 && !strcmp(argv[1], "--help")) {
            fmt::print("Usage: {0} <input> [--test|--help]\n", argv[0]);
            fmt::print("\t - if no argument is provided, STDIN is used as input and result is printed to STDOUT\n");
            fmt::print("\t - if long opt is provided:\n");
            fmt::print("\t\t\t --help     displays this menu\n");
            fmt::print(
                    "\t\t\t --test     runs all defined tests. Exit code will reflect whether all tests succeeded or not.\n");
            fmt::print(
                    "\t - if a file path is provided as argument, it will be used as input and result will be printed to STDOUT\n");
        } else if (argc > 1) {
            std::ifstream fin = std::ifstream(argv[1]);
            std::istream& in = fin;
            run(in, std::cout);
        } else {
            run(std::cin, std::cout);
        }
        return 0;
    }


    bool solution::run_tests() {
        int failedTestsCount = 0;

        for (auto& test : tests) {
            if (!run_test(test)) {
                failedTestsCount++;
            }
        }

        if (failedTestsCount > 0) {
            fmt::print(stderr, "\n{} tests failed.\n", failedTestsCount);
            return false;
        } else {
            fmt::print(stderr, "\nAll {} tests passed!\n", tests.size());
            return true;
        }
    }

    template<typename T>
    std::vector<T> elements_from_stream(std::istream& in) {
        T i{};
        std::vector<T> numbers;
        while (in >> i) {
            numbers.push_back(i);
        }
        return numbers;
    }

    template<typename T>
    std::vector<T> csv_elements_from_stream(std::istream& in) {
        std::vector<T> elements;
        long i;
        char comma;
        while (in >> i) {
            elements.push_back(i);
            in >> comma;
        }
        return elements;
    }

    std::vector<std::string> split_string(const std::string& in, std::string delimiter) {
        std::vector<std::string> result{};
        size_t last{0}, next;
        while ((next = in.find(delimiter, last)) != std::string::npos) {
            result.push_back(in.substr(last, next - last));
            last = next + 1;
        }
        result.push_back(in.substr(last));
        return result;
    }

    template<typename T>
    class permutations {
    public:
        permutations(std::initializer_list<T> data_) : data(data_) {
            std::sort(data.begin(), data.end());
        }

        [[nodiscard]] std::vector<T> get() const {
            return data;
        }

        bool next() {
            return std::next_permutation(data.begin(), data.end());
        }

    private:
        std::vector<T> data;
    };

    std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ") {
        str.erase(0, str.find_first_not_of(chars));
        return str;
    }

    std::string ltrim(std::string&& str, const std::string& chars = "\t\n\v\f\r ") {
        str.erase(0, str.find_first_not_of(chars));
        return str;
    }

    std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ") {
        str.erase(str.find_last_not_of(chars) + 1);
        return str;
    }

    std::string rtrim(std::string&& str, const std::string& chars = "\t\n\v\f\r ") {
        str.erase(str.find_last_not_of(chars) + 1);
        return str;
    }

    std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ") {
        return ltrim(rtrim(str, chars), chars);
    }

    std::string trim(std::string&& str, const std::string& chars = "\t\n\v\f\r ") {
        return ltrim(rtrim(str, chars), chars);
    }
}


#endif
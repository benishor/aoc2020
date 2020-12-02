Regular flow
===
1) edit file `dayX/main.cpp`
2) include `solution.h` and write a class inheriting `aoc::solution`
3) implement solution in `run()`
4) write tests to assert the desired behaviour in `register_tests()`
5) run tests by calling the binary with ` --test`
6) run solution on STDIN data by calling the binary with no arguments
7) run solution on data stored in a file by calling the binary with `path/to/data` argument 

```
#include "solution.h"

// Inherit Solution in order to benefit from boilerplate reduction. 
class day1 : public solution {
public:

    void run(std::istream& in, std::ostream& out) override {
        // Here is where the main problem solving algorithm lives.
        // Read input data from "in" and write results to "out"
    }

    void register_tests() override {
        add_test({"test name 1", "given input", "expected output"});
        add_test({"test name 2", "given input", "expected output"});
        ...
    } 
}

int main() {
    day1 solution;
    return solution.execute(argc, argv);
}
```

Running tests
===
```
./build
./run 1 --test
```

Running solution on "input" file residing in the day1/ folder
===
```
./build
./run 1 
```
Running solution on a particular input file
===
```
./build
./run 1 path/to/input/file
```





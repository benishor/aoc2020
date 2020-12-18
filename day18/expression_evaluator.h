//
// Created by benny on 18/12/2020.
//

#ifndef AOC2K20_EXPRESSION_EVALUATOR_H
#define AOC2K20_EXPRESSION_EVALUATOR_H

#include <functional>
#include <utility>

// TODO: add proper error handling for validating expression syntax
template<typename T>
class expression_evaluator {
public:
    explicit expression_evaluator(std::function<int(char)> opPrecedence) : op_precedence(std::move(opPrecedence)) {}

    T evaluate(const std::string& expression) {
        std::stack<long> operands;
        std::stack<char> operators;

        for (size_t i = 0; i < expression.length(); i++) {
            auto c = expression[i];
            if (std::isspace(c)) {
                continue;
            } else if (c == '(') {
                operators.push(c);
            } else if (std::isdigit(c)) {
                T value = 0;
                while (i < expression.length() && std::isdigit(expression[i])) {
                    value *= 10;
                    value += expression[i] - '0';
                    i++;
                }
                operands.push(value);
                i--;
            } else if (c == ')') {
                while (!operators.empty() && operators.top() != '(') {
                    apply_top_operator(operands, operators);
                }
                operators.pop(); // pop ( from the stack
            } else {
                // must be an operator
                while (!operators.empty() && op_precedence(operators.top()) >= op_precedence(c)) {
                    apply_top_operator(operands, operators);
                }
                operators.push(c);
            }
        }
        while (!operators.empty()) {
            apply_top_operator(operands, operators);
        }
        return operands.top();
    }

    inline void apply_top_operator(std::stack<long>& operands, std::stack<char>& operators) {
        auto op = operators.top();
        operators.pop();
        auto b = operands.top();
        operands.pop();
        auto a = operands.top();
        operands.pop();
        operands.push(apply_operation(op, a, b));
    }

private:
    std::function<int(char)> op_precedence;

    T apply_operation(char operation, T a, T b) {
        switch (operation) {
            case '+':
                return a + b;
            case '*':
                return a * b;
            case '-':
                return a - b;
            case '/':
                return a / b;
            default:
                return 0;
        }
    }
};

#endif //AOC2K20_EXPRESSION_EVALUATOR_H

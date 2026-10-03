//
// Created by Xomagat on 15.09.2026.
//

#ifndef KID_UNARYEXPRESSION_H
#define KID_UNARYEXPRESSION_H

#pragma once
#include <memory>
#include <string>

#include "../../libs/Environment.h"

#include "Expression.h"
#include "../../libs/Value.h"
#include "../../libs/NumberValue.h"

class UnaryExpression : public Expression
{
private:
    std::unique_ptr<Expression> expr;
    char op;

public:
    explicit UnaryExpression(char op, std::unique_ptr<Expression> expr) : op(op), expr(std::move(expr)) {}

    std::unique_ptr<Value> eval(Environment& env) const override
    {
        switch (op)
        {
            case '-': return std::make_unique<NumberValue>(-expr->eval(env)->as_number());
            case '+': return std::make_unique<NumberValue>(expr->eval(env)->as_number());
            case 'f': {
                case '!': {
                    double n = expr->eval(env)->as_number();

                    if (n < 0 || n != std::floor(n))
                        throw std::runtime_error("Факториал есть только у целых чисел от нуля!");
                    if (n > 170)
                        throw std::runtime_error("Слишком большое число для факториала!");

                    double result = 1;
                    for (int i = 2; i <= static_cast<int>(n); i++)
                        result *= i;

                    return std::make_unique<NumberValue>(result);
                }
                case 'n': return std::make_unique<BoolValue>(!expr->eval(env)->as_bool());
            }
            default: throw std::runtime_error("Неопределенное поведение для " + expr->eval(env)->as_string() + "!");
        }
    }
};

#endif // KID_UNARYEXPRESSION_H

//
// Created by Xomagat on 15.09.2026.
//

#ifndef KID_BINEXPRESSION_H
#define KID_BINEXPRESSION_H

#pragma once
#include <cmath>
#include <memory>

#include "AssignmentStatement.h"

#include "../../libs/Environment.h"

#include "Expression.h"
#include "../../libs/NumberValue.h"

class BinExpression : public Expression
{
private:
    char op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

public:
    explicit BinExpression(char op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right) : op(op),
                                                                                                    left(std::move(left)),
                                                                                                    right(std::move(right)) {};

    std::unique_ptr<Value> eval(Environment& env) const override
    {
        if (match_type("текст", left->eval(env).get()))
        {
            std::string s1 = left->eval(env)->as_string();
            std::string s2 = right->eval(env)->as_string();

            switch (op)
            {
                case '+': return std::make_unique<StringValue>(s1 + s2);
                case '*': {
                    std::string r = "";

                    for (long long i = 0; i < std::stoll(s2); i++)
                    {
                        r += s1;
                    }

                    return std::make_unique<StringValue>(r);
                }
            }
        }

        double l = left->eval(env)->as_number();
        double r = right->eval(env)->as_number();

        switch (op)
        {
            case '+': return std::make_unique<NumberValue>(l + r);
            case '-': return std::make_unique<NumberValue>(l - r);
            case '*': return std::make_unique<NumberValue>(l * r);
            case '/': return std::make_unique<NumberValue>(l / r);
            case '%': return std::make_unique<NumberValue>(std::fmod(l, r));
            case 'p': return std::make_unique<NumberValue>(std::pow(l, r));
        }
    }
};

#endif // KID_BINEXPRESSION_H

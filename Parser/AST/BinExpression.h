//
// Created by Xomagat on 15.09.2026.
//

#ifndef KID_BINEXPRESSION_H
#define KID_BINEXPRESSION_H

#pragma once
#include <cmath>
#include <memory>

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
        double l = left->eval(env)->as_number();
        double r = right->eval(env)->as_number();

        switch (op)
        {
            case '+': return std::make_unique<NumberValue>(l + r);
            case '-': return std::make_unique<NumberValue>(l - r);
            case '*': return std::make_unique<NumberValue>(l * r);
            case '/': return std::make_unique<NumberValue>(l / r);
            case 'p': return std::make_unique<NumberValue>(std::pow(l, r));
        }
    }
};

#endif // KID_BINEXPRESSION_H

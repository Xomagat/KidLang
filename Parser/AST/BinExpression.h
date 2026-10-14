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
        auto lv = left->eval(env);
        auto rv = right->eval(env);

        bool ls = dynamic_cast<StringValue*>(lv.get()) != nullptr;
        bool rs = dynamic_cast<StringValue*>(rv.get()) != nullptr;

        if (ls)
        {
            std::string s1 = lv->as_string();
            if (op == '+') return std::make_unique<StringValue>(s1 + rv->as_string());
            if (op == '*')
            {
                long long n = (long long)rv->as_number();
                if (n < 0) throw std::runtime_error("Нельзя повторить текст отрицательное число раз!");
                std::string r;
                for (long long i = 0; i < n; i++) r += s1;
                return std::make_unique<StringValue>(std::move(r));
            }
            throw std::runtime_error("С текстом можно делать только «+» (склеить) и «*» (повторить)!");
        }
        if (rs)
            throw std::runtime_error("Нельзя смешивать число и текст! Преврати текст в число: преврати ... в число");

        double l = lv->as_number(), r = rv->as_number();

        if ((op == '/' || op == '%' || op == 'd') && r == 0)
            throw std::runtime_error("Делить на ноль нельзя!");

        switch (op)
        {
        case '+': return std::make_unique<NumberValue>(l + r);
        case '-': return std::make_unique<NumberValue>(l - r);
        case '*': return std::make_unique<NumberValue>(l * r);
        case '/': return std::make_unique<NumberValue>(l / r);
        case '%': return std::make_unique<NumberValue>(std::fmod(l, r));
        case 'p': return std::make_unique<NumberValue>(std::pow(l, r));
        case 'd': return std::make_unique<NumberValue>(std::floor(l / r));
        }
        throw std::runtime_error("Я не знаю такого действия!");
    }
};

#endif // KID_BINEXPRESSION_H

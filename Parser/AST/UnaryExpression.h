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
        auto v = expr->eval(env);
        switch (op)
        {
        case '-': return std::make_unique<NumberValue>(-v->as_number());
        case '+': return std::make_unique<NumberValue>(v->as_number());
        case 'n': return std::make_unique<BoolValue>(!v->as_bool());
        case 'f': {
            double n = v->as_number();
            if (n < 0 || n != std::floor(n))
                throw std::runtime_error("Факториал бывает только у целых чисел от нуля: 0, 1, 2, 3...");
            if (n > 170)
                throw std::runtime_error("Такое большое число я для факториала не потяну (максимум 170)!");
            double result = 1;
            for (int i = 2; i <= (int)n; i++) result *= i;
            return std::make_unique<NumberValue>(result);
        }
        }
        throw std::runtime_error("Я не знаю такого действия!");
    }
};

#endif // KID_UNARYEXPRESSION_H

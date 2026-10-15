//
// Created by Xomagat on 19.09.2026.
//

#ifndef KID_CONDITIONALEXPRESSION_H
#define KID_CONDITIONALEXPRESSION_H

#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "../../libs/BoolValue.h"
#include "../../libs/NumberValue.h"
#include "../../libs/StringValue.h"
#include "../../libs/Value.h"
#include "Expression.h"

class ConditionalExpression : public Expression {
private:
    std::unique_ptr<Expression> expr1, expr2;
    std::string op;

public:
    explicit ConditionalExpression(std::string op,
        std::unique_ptr<Expression> expr1,
        std::unique_ptr<Expression> expr2) : op(op), expr1(std::move(expr1)), expr2(std::move(expr2))
    {

    }

    std::unique_ptr<Value> eval(Environment& env) const override
    {
        auto v1 = expr1->eval(env);

        if (op == "и")   return std::make_unique<BoolValue>(v1->as_bool() && expr2->eval(env)->as_bool());
        if (op == "или") return std::make_unique<BoolValue>(v1->as_bool() || expr2->eval(env)->as_bool());

        auto v2 = expr2->eval(env);

        bool n1 = dynamic_cast<NothingValue*>(v1.get()) != nullptr;
        bool n2 = dynamic_cast<NothingValue*>(v2.get()) != nullptr;
        if ((n1 || n2) && (op == "==" || op == "!="))
            return std::make_unique<BoolValue>((op == "==") == (n1 && n2));

        if (dynamic_cast<StringValue*>(v1.get()))
        {
            if (op == "==") return std::make_unique<BoolValue>(v1->as_string() == v2->as_string());
            if (op == "!=") return std::make_unique<BoolValue>(v1->as_string() != v2->as_string());
            throw std::runtime_error("Тексты можно сравнивать только знаками «==» и «!=»!");
        }

        double a = v1->as_number(), b = v2->as_number();
        if (op == "==") return std::make_unique<BoolValue>(a == b);
        if (op == "!=") return std::make_unique<BoolValue>(a != b);
        if (op == "<")  return std::make_unique<BoolValue>(a < b);
        if (op == ">")  return std::make_unique<BoolValue>(a > b);
        if (op == "<=") return std::make_unique<BoolValue>(a <= b);
        if (op == ">=") return std::make_unique<BoolValue>(a >= b);
        throw std::runtime_error("Я не знаю такого знака сравнения!");
    }
};

#endif //KID_CONDITIONALEXPRESSION_H

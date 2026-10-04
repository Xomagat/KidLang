//
// Created by Xomagat on 04.10.2026.
//

#ifndef KID_CONVERTEXPRESSION_H
#define KID_CONVERTEXPRESSION_H

#pragma once
#include <memory>
#include <string>
#include <stdexcept>

#include "Expression.h"
#include "../../libs/BoolValue.h"
#include "../../libs/NumberValue.h"
#include "../../libs/StringValue.h"

class ConvertExpression : public Expression
{
private:
    std::string type;
    std::unique_ptr<Expression> expr;

public:
    ConvertExpression(std::string type, std::unique_ptr<Expression> expr)
        : type(std::move(type)), expr(std::move(expr)) {}

    std::unique_ptr<Value> eval(Environment& env) const override
    {
        std::unique_ptr<Value> v = expr->eval(env);
        bool is_string = dynamic_cast<StringValue*>(v.get()) != nullptr;

        if (type == "число")
        {
            if (!is_string)
                return std::make_unique<NumberValue>(v->as_number());

            std::string s = v->as_string();
            try
            {
                size_t used = 0;
                double d = std::stod(s, &used);
                if (used != s.size()) throw std::invalid_argument(s);
                return std::make_unique<NumberValue>(d);
            }
            catch (const std::exception&)
            {
                throw std::runtime_error("Я не могу превратить \"" + s + "\" в число!");
            }
        }

        if (type == "текст")
            return std::make_unique<StringValue>(v->as_string());

        if (type == "условие")
        {
            if (is_string)
            {
                std::string s = v->as_string();
                if (s == "верно")   return std::make_unique<BoolValue>(true);
                if (s == "неверно") return std::make_unique<BoolValue>(false);
                throw std::runtime_error("Я не могу превратить \"" + s + "\" в ответ! Нужно \"да\" или \"нет\".");
            }
            return std::make_unique<BoolValue>(v->as_bool());
        }

        throw std::runtime_error("Я не умею превращать в " + type + "!");
    }
};

#endif // KID_CONVERTEXPRESSION_H

//
// Created by Xomagat on 16.09.2026.
//

#ifndef KID_ASSIGNMENTSTATEMENT_H
#define KID_ASSIGNMENTSTATEMENT_H

#pragma once
#include "Statement.h"
#include "Expression.h"

#include "../../libs/Environment.h"
#include "../../libs/Value.h"

#include "../../libs/BoolValue.h"
#include "../../libs/ListValue.h"
#include "../../libs/NumberValue.h"
#include "../../libs/StringValue.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

inline std::unordered_map<std::string, std::function<std::unique_ptr<Value>(Value*)>> coercers = {
    {"число", [](Value* v) -> std::unique_ptr<Value> {
        auto n = dynamic_cast<NumberValue*>(v);
        if (!n) return nullptr;
        double d = n->as_number();
        return std::make_unique<NumberValue>(d);
    }},
    {"условие", [](Value* v) -> std::unique_ptr<Value> {
        auto n = dynamic_cast<BoolValue*>(v);
        if (!n) return nullptr;
        bool b = n->as_bool();
        return std::make_unique<BoolValue>(b);
    }},
    {"текст", [](Value* v) -> std::unique_ptr<Value> {
        auto n = dynamic_cast<StringValue*>(v);
        if (!n) return nullptr;
        std::string str = n->as_string();
        return std::make_unique<StringValue>(str);
    }},
};

inline bool match_type(const std::string& type, Value* expr)
{
    if (type.rfind("список", 0) == 0)
    {
        auto l = dynamic_cast<ListValue*>(expr);
        if (!l) return false;
        return l->elem_type().empty() || l->elem_type() == list_elem_type(type);
    }

    std::unordered_map<std::string, bool> match = {
        {"текст",    dynamic_cast<StringValue*>(expr) != nullptr},
        {"условие",  dynamic_cast<BoolValue*>(expr) != nullptr},
        {"число",    dynamic_cast<NumberValue*>(expr) != nullptr},
    };

    return match.at(type);
}

class AssignmentStatement : public Statement
{
private:
    std::string type, name;
    std::unique_ptr<Expression> expression;

public:
    explicit AssignmentStatement(std::string type, std::string name, std::unique_ptr<Expression> expr) : type(std::move(type)), name(std::move(name)), expression(std::move(expr)) {
    }

    void execute(Environment& env) const override
    {
        std::unique_ptr<Value> result;

        if (expression)
        {
            result = expression->eval(env);
            if (!type.empty() && !match_type(type, result.get()))
                throw std::runtime_error("Неправильный тип для значения!");
        }

        if (type.empty())
        {
            if (!env.assign(name, std::move(result)))
                throw std::runtime_error("Переменная " + name + " не найдена!");
            return;
        }

        if (!match_type(type, result.get()))
            throw std::runtime_error("Неправильный тип для значения!");

        std::unique_ptr<Value> stored;
        if (auto it = coercers.find(type); it != coercers.end())
            stored = it->second(result.get());
        else
            stored = std::move(result);
        if (auto* l = dynamic_cast<ListValue*>(stored.get()))
            l->set_elem_type(list_elem_type(type));
        env.define(type, name, std::move(stored));
    }
};

#endif // KID_ASSIGNMENTSTATEMENT_H

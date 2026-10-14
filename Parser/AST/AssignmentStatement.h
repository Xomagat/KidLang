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

inline bool match_type(const std::string& type, Value* v)
{
    if (!v) return false;
    if (type.rfind("список", 0) == 0)
    {
        auto l = dynamic_cast<ListValue*>(v);
        return l && (l->elem_type().empty() || l->elem_type() == list_elem_type(type));
    }
    if (type == "текст")   return dynamic_cast<StringValue*>(v) != nullptr;
    if (type == "условие") return dynamic_cast<BoolValue*>(v) != nullptr;
    if (type == "число")   return dynamic_cast<NumberValue*>(v) != nullptr;
    return false;
}

inline std::runtime_error wrong_type_error(const std::string& var, const std::string& type, Value* v)
{
    std::string got = type_name(v);
    std::string msg = "В переменной «" + var + "» должно лежать «" + type + "», а ты кладёшь «" + got + "»!";
    if (type == "число" && got == "текст")
        msg += " Если нужно превратить текст в число, напиши: преврати ... в число";
    return std::runtime_error(msg);
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
        std::unique_ptr<Value> result = expression->eval(env);

        if (type.empty())
        {
            Val* old = env.revolve(name);
            if (!old)
                throw std::runtime_error("Я не знаю переменную «" + name
                    + "». Сначала создай её, например: число " + name + " = 0");
            if (!match_type(old->type, result.get()))
                throw wrong_type_error(name, old->type, result.get());
            if (auto* l = dynamic_cast<ListValue*>(result.get()))
                l->set_elem_type(list_elem_type(old->type));
            env.assign(name, std::move(result));
            return;
        }

        if (!match_type(type, result.get()))
            throw wrong_type_error(name, type, result.get());

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

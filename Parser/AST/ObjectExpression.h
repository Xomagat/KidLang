//
// Created by Xomagat on 18.10.2026.
//

#ifndef KID_OBJECTEXPRESSION_H
#define KID_OBJECTEXPRESSION_H

#pragma once
#include "../../libs/Classes.h"
#include "Expression.h"
#include "Statement.h"

class FieldExpression : public Expression
{
    std::unique_ptr<Expression> target;
    std::string name;
public:
    FieldExpression(std::unique_ptr<Expression> t, std::string n)
        : target(std::move(t)), name(std::move(n)) {}

    std::unique_ptr<Expression> take_target() { return std::move(target); }
    const std::string& field_name() const { return name; }

    std::unique_ptr<Value> eval(Environment& env) const override
    {
        auto t = target->eval(env);
        auto* o = dynamic_cast<ObjectValue*>(t.get());
        if (!o) throw std::runtime_error("Точка работает только с объектом, а тут «" + type_name(t.get()) + "»!");
        auto it = o->data().find(name);
        if (it == o->data().end())
            throw std::runtime_error("У класса «" + o->class_name() + "» нет поля «" + name + "»!");
        return it->second.value->clone();
    }
};

class FieldAssignStatement : public Statement
{
    std::unique_ptr<Expression> target, value;
    std::string name;
public:
    FieldAssignStatement(std::unique_ptr<Expression> t, std::string n, std::unique_ptr<Expression> v)
        : target(std::move(t)), value(std::move(v)), name(std::move(n)) {}

    void execute(Environment& env) const override
    {
        auto t = target->eval(env);
        auto* o = dynamic_cast<ObjectValue*>(t.get());
        if (!o) throw std::runtime_error("Точка работает только с объектом, а тут «" + type_name(t.get()) + "»!");
        auto it = o->data().find(name);
        if (it == o->data().end())
            throw std::runtime_error("У класса «" + o->class_name() + "» нет поля «" + name
                + "». Поля пишутся прямо в классе!");

        auto v = value->eval(env);
        if (!match_type(it->second.type, v.get()))
            throw wrong_type_error(name, it->second.type, v.get());
        adopt_type(v.get(), it->second.type);
        it->second.value = std::move(v);
    }
};

#endif
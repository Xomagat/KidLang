//
// Created by Xomagat on 12.10.2026.
//

#ifndef KID_LISTEXPRESSION_H
#define KID_LISTEXPRESSION_H

#pragma once
#include "../../libs/ListValue.h"
#include "BreakStatement.h"
#include "ContinueStatement.h"
#include "Expression.h"

class ListExpression : public Expression
{
    std::vector<std::unique_ptr<Expression>> items;
public:
    void add(std::unique_ptr<Expression> e) { items.push_back(std::move(e)); }

    std::unique_ptr<Value> eval(Environment& env) const override
    {
        auto list = std::make_unique<ListValue>();
        for (const auto& e : items)
            list->push(e->eval(env));
        return list;
    }
};

class IndexExpression : public Expression
{
    std::unique_ptr<Expression> target, index;
public:
    IndexExpression(std::unique_ptr<Expression> t, std::unique_ptr<Expression> i)
        : target(std::move(t)), index(std::move(i)) {}

    std::unique_ptr<Value> eval(Environment& env) const override
    {
        auto t = target->eval(env);
        auto* list = dynamic_cast<ListValue*>(t.get());
        if (!list) throw std::runtime_error("Квадратные скобки работают только со списком!");

        double d = index->eval(env)->as_number();
        if (d != std::floor(d)) throw std::runtime_error("Номер в списке должен быть целым числом, например 0, 1, 2!");
        long long i = (long long)d;
        if (i < 0 || i >= (long long)list->data().size())
            throw std::runtime_error("В списке всего " + std::to_string(list->data().size()) + " "
                + elements_word(list->data().size()) + ", а ты просишь номер " + std::to_string(i)
                + "! Номера начинаются с 0.");
        return list->data()[i]->clone();
    }
};

class IndexAssignStatement : public Statement
{
    std::string name;
    std::unique_ptr<Expression> index, value;
public:
    IndexAssignStatement(std::string n, std::unique_ptr<Expression> i, std::unique_ptr<Expression> v)
        : name(std::move(n)), index(std::move(i)), value(std::move(v)) {}

    void execute(Environment& env) const override
    {
        Val* var = env.revolve(name);
        if (!var) throw std::runtime_error("Я не знаю переменную «" + name + "». Сначала создай её!");
        auto* list = dynamic_cast<ListValue*>(var->value.get());
        if (!list) throw std::runtime_error("«" + name + "» не список, а «" + type_name(var->value.get())
                                                + "». Квадратные скобки работают только со списком!");

        long long i = (long long)index->eval(env)->as_number();
        if (i < 0 || i >= (long long)list->data().size())
            throw std::runtime_error("В списке «" + name + "» всего " + std::to_string(list->data().size())
                + " " + elements_word(list->data().size()) + ", а ты просишь номер " + std::to_string(i) + "!");


        auto v = value->eval(env);
        list->check(v.get());
        list->data()[i] = std::move(v);
    }
};

class ForEachStatement : public Statement
{
    std::string type, name;
    std::unique_ptr<Expression> list_expr;
    std::unique_ptr<Statement> body;
public:
    ForEachStatement(std::string t, std::string n, std::unique_ptr<Expression> l, std::unique_ptr<Statement> b)
        : type(std::move(t)), name(std::move(n)), list_expr(std::move(l)), body(std::move(b)) {}

    void execute(Environment& env) const override
    {
        auto value = list_expr->eval(env);
        auto* list = dynamic_cast<ListValue*>(value.get());
        if (!list) throw std::runtime_error("После «из» должен быть список!");
        if (!list->elem_type().empty() && list->elem_type() != type)
            throw std::runtime_error("В этом списке лежит «" + list->elem_type() + "», а ты написал «" + type + "»!");

        // снимок, чтобы можно было менять список прямо в цикле
        std::vector<std::unique_ptr<Value>> snapshot;
        for (auto& it : list->data()) snapshot.push_back(it->clone());

        for (auto& item : snapshot)
        {
            Environment loop_env(&env);
            loop_env.define(type, name, item->clone());
            try { body->execute(loop_env); }
            catch (const BreakStatement&)    { break; }
            catch (const ContinueStatement&) { continue; }
        }
    }
};

#endif // KID_LISTEXPRESSION_H

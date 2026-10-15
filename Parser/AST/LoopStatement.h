//
// Created by Xomagat on 20.09.2026.
//

#ifndef KID_LOOPSTATEMENT_H
#define KID_LOOPSTATEMENT_H

#pragma once
#include <memory>

#include "BinExpression.h"
#include "Expression.h"

#include "Statement.h"
#include "ContinueStatement.h"
#include "BreakStatement.h"

class RepeatStatement : public Statement
{
private:
    std::unique_ptr<Expression> how_much;
    std::unique_ptr<Statement> body;

public:
    explicit RepeatStatement(std::unique_ptr<Expression> how_much, std::unique_ptr<Statement> body) : how_much(std::move(how_much)), body(std::move(body)) {}

    void execute(Environment &env) const override
    {
        double c = how_much->eval(env)->as_number();
        for (int i = 0; i < c; i++)
        {
            try
            {
                body->execute(env);
            }
            catch (const BreakStatement&)
            {
                break;
            }
            catch (const ContinueStatement&)
            {
                continue;
            }
        }
    }
};

class WhileStatement : public Statement
{
private:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;

public:
    explicit WhileStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    void execute(Environment &env) const override
    {
        while (condition->eval(env)->as_bool())
        {
            try
            {
                body->execute(env);
            }
            catch (const BreakStatement&)
            {
                break;
            }
            catch (const ContinueStatement&)
            {
                continue;
            }
        }
    }
};

class ForStatement : public Statement
{
private:
    std::unique_ptr<Statement> var;
    std::unique_ptr<Statement> body;
    std::unique_ptr<Expression> end;
    std::string type, name;
    double step;

public:
    explicit ForStatement(std::unique_ptr<Statement> var, std::unique_ptr<Statement> body,
                          std::unique_ptr<Expression> end, std::string type,
                          std::string name, double step)
        : var(std::move(var)), body(std::move(body)), end(std::move(end)),
          type(std::move(type)), name(std::move(name)), step(step) {}

    void execute(Environment& env) const override
    {
        if (type != "число")
            throw std::runtime_error("Цикл «для ... от ... до» работает только с числами. Для списка пиши: для число x из список");
        if (step == 0)
            throw std::runtime_error("Шаг в цикле «для» не может быть нулём, иначе я никогда не закончу!");

        Environment loop_env(&env);
        var->execute(loop_env);

        while (true)
        {
            double i = loop_env.revolve(name)->value->as_number();
            double limit = end->eval(loop_env)->as_number();

            if (step > 0 ? i > limit : i < limit)
                break;

            try
            {
                body->execute(loop_env);
            }
            catch (const BreakStatement&)    { break; }
            catch (const ContinueStatement&) {  }

            double next = loop_env.revolve(name)->value->as_number() + step;
            loop_env.assign(name, std::make_unique<NumberValue>(next));
        }
    }
};

#endif // KID_LOOPSTATEMENT_H

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
    double how_much;
    std::unique_ptr<Statement> body;

public:
    explicit RepeatStatement(double how_much, std::unique_ptr<Statement> body) : how_much(how_much), body(std::move(body)) {}

    void execute(Environment &env) const override
    {
        for (int i = 0; i < how_much; i++)
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
            throw std::runtime_error("Цикл 'для' работает только с числами!");

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
            catch (const ContinueStatement&) { continue; }

            double next = loop_env.revolve(name)->value->as_number() + step;
            loop_env.assign(name, std::make_unique<NumberValue>(next));
        }
    }
};

#endif // KID_LOOPSTATEMENT_H

//
// Created by Xomagat on 19.09.2026.
//

#ifndef KID_IFSTATEMENT_H
#define KID_IFSTATEMENT_H

#pragma once
#include <memory>
#include <vector>

#include "../../libs/Environment.h"
#include "Expression.h"
#include "Statement.h"

struct IfBranch
{
    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> body;
};

class IfStatement : public Statement
{
private:
    std::vector<std::pair<std::unique_ptr<Expression>, std::unique_ptr<Statement>>> branches;
    std::unique_ptr<Statement> else_body;

public:
    IfStatement(std::vector<std::pair<std::unique_ptr<Expression>, std::unique_ptr<Statement>>> branches,
                std::unique_ptr<Statement> else_body)
        : branches(std::move(branches)), else_body(std::move(else_body)) {}

    void execute(Environment& env) const override
    {
        for (const auto& [condition, body] : branches)
        {
            if (condition->eval(env)->as_bool())
            {
                body->execute(env); // BlockStatement сам создаёт свой scope
                return;
            }
        }

        if (else_body)
            else_body->execute(env);
    }
};

class BlockStatement : public Statement
{
private:
    std::vector<std::unique_ptr<Statement>> statements;

public:
    explicit BlockStatement(std::vector<std::unique_ptr<Statement>> statements) : statements(std::move(statements)) {}

    void execute(Environment& env) const override
    {
        Environment local(&env);
        for (auto& s : statements)
        {
            s->execute(local);
        }
    }
};

#endif //KID_IFSTATEMENT_H

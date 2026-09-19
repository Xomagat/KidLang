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
    std::vector<IfBranch> branches;
    std::vector<std::unique_ptr<Statement>> else_body;

public:
    IfStatement(std::vector<IfBranch> branches, std::vector<std::unique_ptr<Statement>> else_body)
        : branches(std::move(branches)), else_body(std::move(else_body)) {}

    void execute(Environment& env) const override
    {
        for (const auto& branch : branches)
        {
            if (branch.condition->eval(env)->as_bool())
            {
                Environment scope(&env);
                for (const auto& stmt : branch.body)
                    stmt->execute(scope);
                return;
            }
        }

        Environment scope(&env);
        for (const auto& stmt : else_body)
            stmt->execute(scope);
    }
};

#endif //KID_IFSTATEMENT_H

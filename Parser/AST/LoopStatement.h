//
// Created by Xomagat on 20.09.2026.
//

#ifndef KID_LOOPSTATEMENT_H
#define KID_LOOPSTATEMENT_H

#pragma once
#include <memory>
#include <vector>

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

#endif // KID_LOOPSTATEMENT_H

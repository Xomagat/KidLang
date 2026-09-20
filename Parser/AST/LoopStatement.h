//
// Created by Xomagat on 20.09.2026.
//

#ifndef KID_LOOPSTATEMENT_H
#define KID_LOOPSTATEMENT_H

#pragma once
#include <memory>
#include <vector>

#include "Statement.h"

class RepeatStatement : public Statement
{
private:
    int how_much;
    std::unique_ptr<Statement> body;

public:
    explicit RepeatStatement(int how_much, std::unique_ptr<Statement> body) : how_much(how_much), body(std::move(body)) {}

    void execute(Environment &env) const override
    {
        for (int i = 0; i < how_much; i++)
        {
            body->execute(env);
        }
    }
};

#endif // KID_LOOPSTATEMENT_H

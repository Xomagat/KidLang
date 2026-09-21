//
// Created by Xomagat on 21.09.2026.
//

#ifndef KID_BREAKSTATEMENT_H
#define KID_BREAKSTATEMENT_H

#pragma once
#include <stdexcept>

#include "Statement.h"

class ControlFlowSignal : public std::runtime_error
{
public:
    explicit ControlFlowSignal(const std::string& what)
        : std::runtime_error(what) {}
};

class BreakStatement : public Statement, public ControlFlowSignal
{
private:

public:
    explicit BreakStatement() : ControlFlowSignal("'стоп' находится за пределом цикла!") {}

    void execute(Environment &env) const override
    {
        throw BreakStatement();
    }
};

#endif // KID_BREAKSTATEMENT_H

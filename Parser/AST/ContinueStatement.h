//
// Created by Xomagat on 21.09.2026.
//

#ifndef KID_CONTINUESTATEMENT_H
#define KID_CONTINUESTATEMENT_H

#pragma once
#include "Statement.h"
#include "BreakStatement.h"

class ContinueStatement : public Statement, public ControlFlowSignal
{
private:

public:
    explicit ContinueStatement() : ControlFlowSignal("'продолжи' находится за пределом цикла!") {}

    void execute(Environment &env) const override
    {
        throw ContinueStatement();
    }
};

#endif // KID_CONTINUESTATEMENT_H

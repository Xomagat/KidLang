//
// Created by Xomagat on 15.09.2026.
//

#ifndef KID_STATEMENT_H
#define KID_STATEMENT_H

#include "../../libs/Environment.h"

class Statement
{
private:

public:
    ~Statement() = default;

    virtual void execute(Environment& env) const = 0;
};

#endif // KID_STATEMENT_H

//
// Created by Xomagat on 16.09.2026.
//

#ifndef KID_VARIABLESEXPRESSION_H
#define KID_VARIABLESEXPRESSION_H

#pragma once
#include <string>

#include "../../libs/Environment.h"
#include "../../libs/Value.h"
#include "Expression.h"

class VariableExpression : public Expression
{
private:
    std::string name;

public:
    explicit VariableExpression(const std::string& name) : name(name) {}

    std::unique_ptr<Value> eval(Environment& env) const override
    {
        const Val* val = env.revolve(name);

        if (!val)
            throw std::runtime_error("Variable {" + name + "} not found!");

        return val->value->clone();
    }
};

#endif // KID_VARIABLESEXPRESSION_H

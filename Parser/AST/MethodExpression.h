//
// Created by Xomagat on 08.10.2026.
//

#ifndef KID_METHODEXPRESSION_H
#define KID_METHODEXPRESSION_H

#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Expression.h"
#include "../../libs/Methods.h"

class MethodExpression : public Expression
{
    std::unique_ptr<Expression> target;
    std::string name;
    std::vector<std::unique_ptr<Expression>> args;

public:
    MethodExpression(std::unique_ptr<Expression> target, std::string name)
        : target(std::move(target)), name(std::move(name)) {}

    void add_arg(std::unique_ptr<Expression> a) { args.push_back(std::move(a)); }

    std::unique_ptr<Value> eval(Environment& env) const override
    {
        std::unique_ptr<Value> self = target->eval(env);

        MethodArgs values;
        for (const auto& a : args) values.push_back(a->eval(env));

        const Method& m = Methods::get(type_of(self.get()), name);
        return m(*self, values);
    }
};

#endif // KID_METHODEXPRESSION_H

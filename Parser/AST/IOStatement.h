//
// Created by Xomagat on 15.09.2026.
//

#ifndef KID_IOSTATEMENT_H
#define KID_IOSTATEMENT_H

#pragma once
#include <iostream>
#include <memory>

#include "../../libs/Environment.h"

#include "Expression.h"
#include "Statement.h"

class ShowStatement : public Statement
{
private:
    std::unique_ptr<Expression> expr;

public:
    explicit ShowStatement(std::unique_ptr<Expression> expr) : expr(std::move(expr)) {}

    void execute(Environment& env) const override
    {
        std::cout << expr->eval(env)->as_string() << std::endl;
    }
};

#endif // KID_IOSTATEMENT_H

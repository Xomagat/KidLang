//
// Created by Xomagat on 15.09.2026.
//

#ifndef KID_EXPRESSION_H
#define KID_EXPRESSION_H

#pragma once
#include <memory>

#include "../../libs/Value.h"
#include "../../libs/Environment.h"

class Expression
{
private:

public:
    ~Expression() = default;

    virtual std::unique_ptr<Value> eval(Environment& env) const = 0;
};

#endif // KID_EXPRESSION_H

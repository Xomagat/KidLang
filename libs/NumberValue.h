//
// Created by Xomagat on 15.09.2026.
//

#ifndef KID_NUMBERVALUE_H
#define KID_NUMBERVALUE_H

#pragma once
#include <string>

#include "Value.h"

class NumberValue : public Value
{
private:
    double value;

public:
    explicit NumberValue(double val) : value(val) {}

    double as_number() const override
    {
        return value;
    }

    std::string as_string() const override
    {
        return std::to_string(value);
    }

    bool as_bool() const override
    {
        return value != 0;
    }

    std::unique_ptr<Value> clone() const override
    {
        return std::make_unique<NumberValue>(value);
    }
};

#endif // KID_NUMBERVALUE_H

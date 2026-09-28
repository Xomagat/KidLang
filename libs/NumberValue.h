//
// Created by Xomagat on 15.09.2026.
//

#ifndef KID_NUMBERVALUE_H
#define KID_NUMBERVALUE_H

#pragma once
#include <string>
#include <sstream>

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
        std::ostringstream ss;
        ss << value;
        return ss.str();
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

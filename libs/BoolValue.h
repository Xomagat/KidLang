//
// Created by Xomagat on 15.09.2026.
//

#ifndef KID_BOOLVALUE_H
#define KID_BOOLVALUE_H

#pragma once
#include <string>

#include "Value.h"

class BoolValue : public Value
{
private:
    bool value;

public:
    explicit BoolValue(bool val) : value(val) {}

    double as_number() const override
    {
        return value == true ? 1 : 0;
    }

    std::string as_string() const override
    {
        return value == true ? "да" : "нет";
    }

    bool as_bool() const override
    {
        return value;
    }

    std::unique_ptr<Value> clone() const override
    {
        return std::make_unique<BoolValue>(value);
    }
};

#endif // KID_BOOLVALUE_H

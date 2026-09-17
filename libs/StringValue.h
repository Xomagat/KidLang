//
// Created by Xomagat on 15.09.2026.
//

#ifndef KID_STRINGVALUE_H
#define KID_STRINGVALUE_H

#pragma once
#include <string>

#include "Value.h"

class StringValue : public Value
{
private:
    std::string value;

public:
    explicit StringValue(std::string val) : value(val) {}

    double as_number() const override
    {
        return std::stod(value);
    }

    std::string as_string() const override
    {
        return value;
    }

    bool as_bool() const override
    {
        return value.compare("");
    }

    std::unique_ptr<Value> clone() const override
    {
        return std::make_unique<StringValue>(value);
    }
};

#endif // KID_STRINGVALUE_H

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
        try
        {
            size_t used = 0;
            double d = std::stod(value, &used);
            if (used != value.size()) throw std::invalid_argument(value);
            return d;
        }
        catch (const std::exception&)
        {
            throw std::runtime_error("Я не могу превратить текст «" + value + "» в число!");
        }
    }
    
    bool as_bool() const override { return !value.empty(); }

    std::string as_string() const override
    {
        return value;
    }

    std::unique_ptr<Value> clone() const override
    {
        return std::make_unique<StringValue>(value);
    }
};

#endif // KID_STRINGVALUE_H

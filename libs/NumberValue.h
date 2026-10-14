//
// Created by Xomagat on 15.09.2026.
//

#ifndef KID_NUMBERVALUE_H
#define KID_NUMBERVALUE_H

#pragma once
#include <string>
#include <charconv>
#include <cmath>
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
        if (std::isnan(value)) return "не число";
        if (std::isinf(value)) return value > 0 ? "бесконечность" : "-бесконечность";

        if (value == std::floor(value) && std::fabs(value) < 1e19)
        {
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%.0f", value);
            return buf;
        }

        char buf[64];
        auto res = std::to_chars(buf, buf + sizeof(buf), value, std::chars_format::general, 15);
        return std::string(buf, res.ptr);
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

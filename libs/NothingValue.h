//
// Created by Xomagat on 02.10.2026.
//

#ifndef KID_NOTHING_H
#define KID_NOTHING_H

#pragma once
#include <memory>
#include <string>

#include "Value.h"

class NothingValue : public Value
{
private:

public:
    explicit NothingValue() {}

    double as_number() const override
    {
        return NULL;
    }

    std::string as_string() const override
    {
        return "ничего";
    }

    bool as_bool() const override
    {
        return NULL;
    }

    std::unique_ptr<Value> clone() const override
    {
        return std::make_unique<NothingValue>();
    }
};

#endif // KID_NOTHING_H

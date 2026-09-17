//
// Created by Xomagat on 15.09.2026.
//

#ifndef KID_VALUE_H
#define KID_VALUE_H

#pragma once
#include <string>
#include <variant>
#include <memory>

class Value
{
private:

public:
    virtual ~Value() = default;

    virtual double as_number() const = 0;
    virtual std::string as_string() const = 0;
    virtual bool as_bool() const = 0;

    virtual std::unique_ptr<Value> clone() const = 0;
};

#endif // KID_VALUE_H

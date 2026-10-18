//
// Created by Xomagat on 18.10.2026.
//

#ifndef KID_OBJECTVALUE_H
#define KID_OBJECTVALUE_H

#pragma once
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "Value.h"

class ObjectValue : public Value
{
public:
    struct Field { std::string type; std::unique_ptr<Value> value; };
    using Fields = std::unordered_map<std::string, Field>;

private:
    std::string cls;
    std::shared_ptr<Fields> fields;

public:
    explicit ObjectValue(std::string cls)
        : cls(std::move(cls)), fields(std::make_shared<Fields>()) {}
    ObjectValue(std::string cls, std::shared_ptr<Fields> f)
        : cls(std::move(cls)), fields(std::move(f)) {}

    const std::string& class_name() const { return cls; }
    Fields& data() { return *fields; }

    double as_number() const override { throw std::runtime_error("Объект нельзя превратить в число!"); }
    bool as_bool() const override { return true; }
    std::string as_string() const override { return "<" + cls + ">"; }
    std::unique_ptr<Value> clone() const override { return std::make_unique<ObjectValue>(cls, fields); }
};

#endif

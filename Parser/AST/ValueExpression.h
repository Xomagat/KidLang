//
// Created by Xomagat on 15.09.2026.
//

#ifndef KID_VALUEEXPRESSION_H
#define KID_VALUEEXPRESSION_H

#pragma once
#include <string>
#include <memory>

#include "../../libs/Environment.h"
#include "../../libs/Value.h"

#include "../../libs/NumberValue.h"
#include "../../libs/StringValue.h"
#include "../../libs/BoolValue.h"
#include "Expression.h"

struct BoolTag { bool b; };

class ValueExpression : public Expression
{
private:
    std::unique_ptr<Value> value;

public:
    explicit ValueExpression(const double& value)
    {
        this->value = std::make_unique<NumberValue>(value);
    }
    explicit ValueExpression(const std::string& value)
    {
        this->value = std::make_unique<StringValue>(value);
    }
    explicit ValueExpression(const BoolTag& value)
    {
        this->value = std::make_unique<BoolValue>(value.b);
    }

    std::unique_ptr<Value> eval(Environment& env) const override
    {
        if (auto b = dynamic_cast<BoolValue*>(value.get()))
            return std::make_unique<BoolValue>(b->as_bool());
        if (auto s = dynamic_cast<StringValue*>(value.get()))
            return std::make_unique<StringValue>(s->as_string());
        return std::make_unique<NumberValue>(value->as_number());
    }
};

#endif // KID_VALUEEXPRESSION_H

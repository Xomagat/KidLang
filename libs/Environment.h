//
// Created by Xomagat on 16.09.2026.
//

#ifndef KID_ENVIRONMENT_H
#define KID_ENVIRONMENT_H

#pragma once
#include <string>
#include <unordered_map>
#include <memory>

#include "Value.h"

struct Val
{
    std::string type;
    std::unique_ptr<Value> value;
};

class Environment
{
private:
    std::unordered_map<std::string, Val> variables;
    Environment* parent;

public:
    explicit Environment(Environment* parent = nullptr) : parent(parent) {}

    void define(const std::string& type, const std::string& name, std::unique_ptr<Value> value)
    {
        variables[name] = {type, std::move(value)};
    }

    bool assign(const std::string& name, std::unique_ptr<Value> value)
    {
        auto it = variables.find(name);
        if (it != variables.end())
        {
            it->second.value = std::move(value);
            return true;
        }
        if (parent != nullptr)
            return parent->assign(name, std::move(value));
        return false;
    }

    Val* revolve(const std::string& name)
    {
        auto it = variables.find(name);
        if (it != variables.end())
            return &it->second;

        if (parent != nullptr)
            return parent->revolve(name);

        return nullptr;
    }

    std::string get_type(const std::string& name)
    {
        auto it = variables.find(name);
        if (it != variables.end())
            return it->second.type;
        if (parent != nullptr)
            return parent->get_type(name);
        return "";
    }
};

#endif // KID_ENVIRONMENT_H

//
// Created by Xomagat on 21.08.2026.
//

#pragma once
#include <cmath>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Value.h"
#include "Function.h"
#include "NumberValue.h"

class Functions
{
private:
    inline static std::unordered_map<std::string, std::unique_ptr<Function>> functions = {
    };

public:
    static bool is_exist(const std::string& name)
    {
        std::vector<std::string> names;

        for (const auto& name : functions)
            names.push_back(name.first);

        return functions.contains(name);
    }

    static Function* get(const std::string& name)
    {
        auto it = functions.find(name);
        if (it == functions.end())
            throw std::runtime_error("Я не знаю рецепт «" + name + "». Ты его написал? Или опечатался?");
        return it->second.get();
    }

    static void define(const std::string& name, std::unique_ptr<Function> function)
    {
        functions[name] = std::move(function);
    }
};

//
// Created by Xomagat on 21.08.2026.
//

#pragma once
#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Value.h"
#include "Function.h"
#include "NumberValue.h"
#include "ListValue.h"

class BuiltinFunction : public Function
{
    using Fn = std::function<std::unique_ptr<Value>(std::vector<std::unique_ptr<Value>>&)>;

    std::string name;
    size_t arity;
    Fn fn;

public:
    BuiltinFunction(std::string name, size_t arity, Fn fn)
        : name(std::move(name)), arity(arity), fn(std::move(fn)) {}

    std::unique_ptr<Value> execute(Environment&, std::vector<std::unique_ptr<Value>> args) const override
    {
        if (args.size() != arity)
            throw std::runtime_error("Рецепту «" + name + "» нужно аргументов: " + std::to_string(arity)
                + ", а ты дал: " + std::to_string(args.size()) + "!");
        return fn(args);
    }
};

class Functions
{
private:
    using Table = std::unordered_map<std::string, std::unique_ptr<Function>>;

    static Table make_builtins()
    {
        Table m;

        m["обрежь_дробное_число"] = std::make_unique<BuiltinFunction>(
            "обрежь_дробное_число", 2,
            [](std::vector<std::unique_ptr<Value>>& a) -> std::unique_ptr<Value> {
                for (size_t i = 0; i < 2; i++)
                    if (!dynamic_cast<NumberValue*>(a[i].get()))
                        throw std::runtime_error("Рецепт «обрежь_дробное_число» ждёт «число» в аргументе номер "
                            + std::to_string(i + 1) + ", а ты дал «" + type_name(a[i].get()) + "»!");

                double x = a[0]->as_number();
                double d = a[1]->as_number();

                if (d != std::floor(d) || d < 0)
                    throw std::runtime_error("Количество знаков после запятой должно быть целым числом от 0, например 2!");
                if (d > 15)
                    throw std::runtime_error("Больше 15 знаков после запятой я точно хранить не умею!");

                double factor = std::pow(10.0, d);
                return std::make_unique<NumberValue>(std::trunc(x * factor) / factor);
            });

        return m;
    }

    inline static Table functions = make_builtins();

public:
    static bool is_exist(const std::string& name) { return functions.contains(name); }

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
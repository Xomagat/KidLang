//
// Created by Xomagat on 08.10.2026.
//

#ifndef KID_METHODS_H
#define KID_METHODS_H

#pragma once
#include <cmath>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "Value.h"
#include "NumberValue.h"
#include "StringValue.h"
#include "BoolValue.h"
#include "NothingValue.h"

using MethodArgs = std::vector<std::unique_ptr<Value>>;
using Method = std::function<std::unique_ptr<Value>(Value& self, MethodArgs& args)>;

inline std::string type_of(Value* v)
{
    if (dynamic_cast<NumberValue*>(v))  return "число";
    if (dynamic_cast<StringValue*>(v))  return "текст";
    if (dynamic_cast<BoolValue*>(v))    return "условие";
    return "ничего";
}

class Methods
{
    using Table = std::unordered_map<std::string, std::unordered_map<std::string, Method>>;

    static void need(const MethodArgs& a, size_t n, const std::string& name)
    {
        if (a.size() != n)
            throw std::runtime_error("Метод '" + name + "' ждёт аргументов: " + std::to_string(n));
    }

    static Table init()
    {
        Table t;

        // ---------- текст ----------
        t["текст"]["длина"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 0, "длина");
            double n = 0;
            for (unsigned char c : s.as_string())
                if ((c & 0xC0) != 0x80) n++;
            return std::make_unique<NumberValue>(n);
        };
        t["текст"]["содержит"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 1, "содержит");
            return std::make_unique<BoolValue>(
                s.as_string().find(a[0]->as_string()) != std::string::npos);
        };
        t["текст"]["заменить"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 2, "заменить");
            std::string str = s.as_string(), from = a[0]->as_string(), to = a[1]->as_string();
            if (from.empty()) return std::make_unique<StringValue>(str);
            size_t p = 0;
            while ((p = str.find(from, p)) != std::string::npos)
            {
                str.replace(p, from.size(), to);
                p += to.size();
            }
            return std::make_unique<StringValue>(str);
        };

        // ---------- число ----------


        return t;
    }

public:
    static const Method& get(const std::string& type, const std::string& name)
    {
        static const Table table = init();

        auto t = table.find(type);
        if (t != table.end())
        {
            auto m = t->second.find(name);
            if (m != t->second.end()) return m->second;
        }
        throw std::runtime_error("У типа «" + type + "» нет метода «" + name + "»!");
    }
};

#endif // KID_METHODS_H

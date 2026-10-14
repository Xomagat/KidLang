//
// Created by Xomagat on 12.10.2026.
//

#ifndef KID_LISTVALUE_H
#define KID_LISTVALUE_H

#pragma once
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "Value.h"
#include "NumberValue.h"
#include "StringValue.h"
#include "BoolValue.h"
#include "NothingValue.h"

inline std::string elements_word(long long n)
{
    long long a = n % 100, b = n % 10;
    if ((a >= 11 && a <= 14) || b == 0 || b >= 5) return "элементов";
    if (b == 1) return "элемент";
    return "элемента";
}

inline std::string type_name(const Value* v);

class ListValue : public Value
{
public:
    using Items = std::vector<std::unique_ptr<Value>>;

private:
    std::shared_ptr<Items> items;
    std::string elem;

public:
    explicit ListValue(std::string elem = "")
        : items(std::make_shared<Items>()), elem(std::move(elem)) {}
    ListValue(std::shared_ptr<Items> items, std::string elem)
        : items(std::move(items)), elem(std::move(elem)) {}

    Items& data() { return *items; }
    const Items& data() const { return *items; }
    const std::string& elem_type() const { return elem; }
    void set_elem_type(const std::string& t) { elem = t; }

    void check(const Value* v)
    {
        std::string t = type_name(v);
        if (elem.empty()) elem = t;
        else if (elem != t)
            throw std::runtime_error("В этом списке лежат «" + elem + "», а ты кладёшь «" + t + "»!");
    }
    void push(std::unique_ptr<Value> v) { check(v.get()); items->push_back(std::move(v)); }

    double as_number() const override { throw std::runtime_error("Список нельзя превратить в число!"); }
    bool as_bool() const override { return !items->empty(); }
    std::string as_string() const override
    {
        std::string r = "[";
        for (size_t i = 0; i < items->size(); i++)
        {
            if (i) r += ", ";
            const Value* v = (*items)[i].get();
            r += dynamic_cast<const StringValue*>(v) ? "\"" + v->as_string() + "\"" : v->as_string();
        }
        return r + "]";
    }
    std::unique_ptr<Value> clone() const override
    {
        return std::make_unique<ListValue>(items, elem);
    }
};

inline std::string type_name(const Value* v)
{
    if (dynamic_cast<const NumberValue*>(v)) return "число";
    if (dynamic_cast<const StringValue*>(v)) return "текст";
    if (dynamic_cast<const BoolValue*>(v))   return "условие";
    if (auto l = dynamic_cast<const ListValue*>(v))
        return l->elem_type().empty() ? "список" : "список " + l->elem_type();
    return "ничего";
}

inline std::string list_elem_type(const std::string& type)
{
    const std::string kw = "список";
    return type.size() > kw.size() ? type.substr(kw.size() + 1) : "";
}

#endif // KID_LISTVALUE_H

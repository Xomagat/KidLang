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

class DictValue : public Value
{
public:
    using Entry = std::pair<std::unique_ptr<Value>, std::unique_ptr<Value>>;
    using Items = std::vector<Entry>;

private:
    std::shared_ptr<Items> items;
    std::string key_t, val_t;

    static std::string show(const Value* v)
    {
        return dynamic_cast<const StringValue*>(v) ? "\"" + v->as_string() + "\"" : v->as_string();
    }

public:
    explicit DictValue(std::string k = "", std::string v = "")
        : items(std::make_shared<Items>()), key_t(std::move(k)), val_t(std::move(v)) {}
    DictValue(std::shared_ptr<Items> items, std::string k, std::string v)
        : items(std::move(items)), key_t(std::move(k)), val_t(std::move(v)) {}

    Items& data() { return *items; }
    const Items& data() const { return *items; }
    const std::string& key_type() const { return key_t; }
    const std::string& value_type() const { return val_t; }
    void set_types(const std::string& k, const std::string& v) { key_t = k; val_t = v; }

    Entry* find(const Value& key)
    {
        const std::string s = key.as_string();
        for (auto& e : *items)
            if (e.first->as_string() == s) return &e;
        return nullptr;
    }

    // для чтения: ничего не запоминает
    void check_key(const Value* k) const
    {
        std::string t = type_name(k);
        if (!key_t.empty() && key_t != t)
            throw std::runtime_error("В этом словаре ключи — «" + key_t + "», а ты используешь «" + t + "»!");
    }

    void put(std::unique_ptr<Value> k, std::unique_ptr<Value> v)
    {
        std::string kt = type_name(k.get()), vt = type_name(v.get());
        if (kt != "число" && kt != "текст")
            throw std::runtime_error("Ключом словаря может быть только число или текст!");
        if (key_t.empty()) key_t = kt;
        else if (key_t != kt)
            throw std::runtime_error("В этом словаре ключи — «" + key_t + "», а ты кладёшь «" + kt + "»!");
        if (val_t.empty()) val_t = vt;
        else if (val_t != vt)
            throw std::runtime_error("В этом словаре лежат «" + val_t + "», а ты кладёшь «" + vt + "»!");

        if (Entry* e = find(*k)) e->second = std::move(v);
        else items->emplace_back(std::move(k), std::move(v));
    }

    bool erase(const Value& key)
    {
        const std::string s = key.as_string();
        for (auto it = items->begin(); it != items->end(); ++it)
            if (it->first->as_string() == s) { items->erase(it); return true; }
        return false;
    }

    double as_number() const override { throw std::runtime_error("Словарь нельзя превратить в число!"); }
    bool as_bool() const override { return !items->empty(); }
    std::string as_string() const override
    {
        std::string r = "{";
        for (size_t i = 0; i < items->size(); i++)
        {
            if (i) r += ", ";
            r += show((*items)[i].first.get()) + ": " + show((*items)[i].second.get());
        }
        return r + "}";
    }
    std::unique_ptr<Value> clone() const override
    {
        return std::make_unique<DictValue>(items, key_t, val_t);
    }
};

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
    if (auto d = dynamic_cast<const DictValue*>(v))
        return d->key_type().empty() ? "словарь" : "словарь " + d->key_type() + " " + d->value_type();
    return "ничего";
}

inline std::string list_elem_type(const std::string& type)
{
    const std::string kw = "список";
    return type.size() > kw.size() ? type.substr(kw.size() + 1) : "";
}

inline std::string dict_key_type(const std::string& t)
{
    const size_t n = std::string("словарь ").size();
    if (t.size() <= n) return "";
    std::string rest = t.substr(n);
    return rest.substr(0, rest.find(' '));
}
inline std::string dict_value_type(const std::string& t)
{
    const size_t n = std::string("словарь ").size();
    if (t.size() <= n) return "";
    std::string rest = t.substr(n);
    size_t p = rest.find(' ');
    return p == std::string::npos ? "" : rest.substr(p + 1);
}

#endif // KID_LISTVALUE_H

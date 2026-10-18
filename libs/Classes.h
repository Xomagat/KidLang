//
// Created by Xomagat on 18.10.2026.
//

#ifndef KID_CLASSES_H
#define KID_CLASSES_H

#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../Parser/AST/AssignmentStatement.h"
#include "../Parser/AST/Expression.h"
#include "Function.h"
#include "ObjectValue.h"

struct FieldDecl
{
    std::string type, name;
    std::shared_ptr<Expression> init;
};

struct ClassInfo
{
    std::string name;
    std::vector<FieldDecl> fields;
    std::unordered_map<std::string, std::shared_ptr<UserDefineFunction>> methods;
};

class Classes
{
    inline static std::unordered_map<std::string, ClassInfo> table;
public:
    static void define(const ClassInfo& c) { table[c.name] = c; }
    static bool exists(const std::string& n) { return table.contains(n); }
    static ClassInfo& get(const std::string& n)
    {
        auto it = table.find(n);
        if (it == table.end()) throw std::runtime_error("Я не знаю класс «" + n + "»!");
        return it->second;
    }
};

inline void adopt_type(Value* v, const std::string& type)
{
    if (auto* l = dynamic_cast<ListValue*>(v)) l->set_elem_type(list_elem_type(type));
    if (auto* d = dynamic_cast<DictValue*>(v)) d->set_types(dict_key_type(type), dict_value_type(type));
}

inline std::unique_ptr<Value> call_method(UserDefineFunction& f, const std::string& name,
                                          ObjectValue& self,
                                          std::vector<std::unique_ptr<Value>> values,
                                          Environment& env)
{
    if (values.size() != (size_t)f.get_names_size())
        throw std::runtime_error("Рецепту «" + name + "» нужно аргументов: "
            + std::to_string(f.get_names_size()) + ", а ты дал: " + std::to_string(values.size()) + "!");

    Environment local(&env.root());
    local.define(self.class_name(), "это", self.clone());

    for (int i = 0; i < f.get_names_size(); i++)
    {
        if (!match_type(f.get_type_index(i), values[i].get()))
            throw std::runtime_error("Рецепт «" + name + "» ждёт «" + f.get_type_index(i)
                + "» в «" + f.get_name_index(i) + "», а ты дал «" + type_name(values[i].get()) + "»!");
        local.define(f.get_type_index(i), f.get_name_index(i), std::move(values[i]));
    }

    auto result = f.execute(local, {});
    return result ? std::move(result) : std::make_unique<NothingValue>();
}

inline std::unique_ptr<Value> instantiate(const std::string& cls,
                                          std::vector<std::unique_ptr<Value>> args,
                                          Environment& env)
{
    ClassInfo& ci = Classes::get(cls);
    auto obj = std::make_unique<ObjectValue>(cls);

    Environment defaults(&env.root());
    for (auto& f : ci.fields)
    {
        auto v = f.init->eval(defaults);
        if (!match_type(f.type, v.get())) throw wrong_type_error(f.name, f.type, v.get());
        adopt_type(v.get(), f.type);
        obj->data()[f.name] = {f.type, std::move(v)};
    }

    auto it = ci.methods.find("создать");
    if (it != ci.methods.end())
        call_method(*it->second, "создать", *obj, std::move(args), env);
    else if (!args.empty())
        throw std::runtime_error("У класса «" + cls + "» нет рецепта «создать», а ты передал аргументы!");

    return obj;
}

#endif
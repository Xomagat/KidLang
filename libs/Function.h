//
// Created by Xomagat on 21.08.2026.
//

#pragma once
#include <memory>
#include <utility>
#include <vector>

#include "Environment.h"
#include "Value.h"

#include "../Parser/AST/Statement.h"
#include "../Parser/AST/ReturnStatement.h"

inline std::string value_type_name(Value* v)
{
    if (dynamic_cast<NumberValue*>(v)) return "число";
    if (dynamic_cast<StringValue*>(v)) return "текст";
    if (dynamic_cast<BoolValue*>(v))   return "условие";
    return "неизвестный тип";
}

class Function
{
private:
public:
    virtual ~Function() = default;

    virtual std::unique_ptr<Value> execute(Environment& env, std::vector<std::unique_ptr<Value>> args) const = 0;
};

class UserDefineFunction : public Function
{
private:
    std::string type;
    std::vector<std::string> arg_types;
    std::vector<std::string> arg_names;
    std::shared_ptr<Statement> body;

public:
    explicit UserDefineFunction(std::string  type, const std::vector<std::string>& arg_types, const std::vector<std::string>& arg_names, std::shared_ptr<Statement> body)
    : type(std::move(type)), arg_types(arg_types), arg_names(arg_names), body(std::move(body)) {}

    const std::string& get_type()
    {
        return type;
    }

    int get_names_size()
    {
        return arg_names.size();
    }

    std::string get_name_index(const int& index)
    {
        if (index < 0 || index >= get_names_size()) return "";
        return arg_names[index];
    }

    std::string get_type_index(const int& index)
    {
        if (index < 0 || index >= get_names_size()) return "";
        return arg_types[index];
    }

    std::unique_ptr<Value> execute(Environment& env, std::vector<std::unique_ptr<Value>> args) const override
    {
        try
        {
            Environment local(&env);
            body->execute(local);
        }
        catch (ReturnException& re)
        {
            std::unique_ptr<Value> result = re.take_value();

            if (type == "ничего")
            {
                if (re.take_value() == nullptr)
                    return result;

                throw std::runtime_error("Этот рецепт ничего не обещал вернуть, а ты вернул «"
                                     + value_type_name(result.get()) + "»!");
            }

            if (value_type_name(result.get()) != type)
                throw std::runtime_error("Рецепт обещал вернуть «" + type + "», а ты вернул «"
                                         + value_type_name(result.get()) + "»!");

            return result;
        }

        if (type != "ничего")
            throw std::runtime_error("Рецепт обещал вернуть «" + type + "», но ничего не вернул!");

        return nullptr;
    }
};
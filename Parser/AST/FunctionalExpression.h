//
// Created by Xomagat on 21.08.2026.
//

#pragma once
#include <string>
#include <vector>

#include "AssignmentStatement.h"

#include "Expression.h"
#include "../../libs/Functions.h"

class FunctionalExpression : public Expression
{
private:
    std::string name;
    std::vector<std::unique_ptr<Expression>> args;

public:
    explicit FunctionalExpression(std::string name) : name(name) {}

    explicit FunctionalExpression(std::string name, std::vector<std::unique_ptr<Expression>> args) : name(name), args(std::move(args)) {}

    void add_arg(std::unique_ptr<Expression> arg)
    {
        args.push_back(std::move(arg));
    }

    std::unique_ptr<Value> eval(Environment& env) const override
    {
        std::vector<std::unique_ptr<Value>> values;
        for (const auto& arg : args) values.push_back(arg->eval(env));

        Function* function = Functions::get(name);

        if (auto f = dynamic_cast<UserDefineFunction*>(function))
        {
            if (values.size() != (size_t)f->get_names_size())
                throw std::runtime_error("Рецепту «" + name + "» нужно аргументов: "
                    + std::to_string(f->get_names_size()) + ", а ты дал: " + std::to_string(values.size()) + "!");

            Environment local(&env.root());

            for (int i = 0; i < f->get_names_size(); i++)
            {
                if (!match_type(f->get_type_index(i), values[i].get()))
                    throw std::runtime_error("Рецепт «" + name + "» ждёт «" + f->get_type_index(i)
                        + "» в «" + f->get_name_index(i) + "», а ты дал «" + type_name(values[i].get()) + "»!");
                local.define(f->get_type_index(i), f->get_name_index(i), std::move(values[i]));
            }

            std::unique_ptr<Value> result = f->execute(local, {});
            if (!result) return std::make_unique<NothingValue>();
            return result;
        }

        return function->execute(env, std::move(values));
    }
};

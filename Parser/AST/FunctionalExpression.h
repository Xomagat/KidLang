//
// Created by Xomagat on 21.08.2026.
//

#pragma once
#include <string>
#include <vector>

#include "AssignmentStatement.h"

#include "Expression.h"
#include "../../libs/Functions.h"
#include "../../libs/Classes.h"

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

        if (Classes::exists(name))
            return instantiate(name, std::move(values), env);

        Function* function = Functions::get(name);

        if (auto f = dynamic_cast<UserDefineFunction*>(function))
        {
            Environment local(&env.root());
            bind_args(*f, name, values, local);

            std::unique_ptr<Value> result = f->execute(local, {});
            if (!result) return std::make_unique<NothingValue>();
            return result;
        }

        return function->execute(env, std::move(values));
    }
};

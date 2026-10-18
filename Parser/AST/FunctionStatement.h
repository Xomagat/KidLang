//
// Created by Xomagat on 21.08.2026.
//

#pragma once
#include <memory>
#include <string>
#include <vector>

#include "FunctionalExpression.h"
#include "Statement.h"

class FunctionStatement : public Statement
{
private:
    std::unique_ptr<Expression> expr;

public:
    explicit FunctionStatement(std::unique_ptr<Expression> expr) : expr(std::move(expr)) {}

    void execute(Environment &env) const override
    {
        expr->eval(env);
    }
};

class FunctionDefineStatement : public Statement
{
private:
    std::string type;
    std::string name;
    std::vector<std::string> arg_names;
    std::vector<std::string> arg_types;
    std::vector<std::shared_ptr<Expression>> arg_defaults;
    std::shared_ptr<Statement> body;

public:
    explicit FunctionDefineStatement(std::string type, std::string name,
        const std::vector<std::string>& arg_types, const std::vector<std::string>& arg_names,
        const std::vector<std::shared_ptr<Expression>>& arg_defaults, std::unique_ptr<Statement> body)
        : type(std::move(type)), name(std::move(name)), arg_names(arg_names), arg_types(arg_types),
          arg_defaults(arg_defaults), body(std::move(body)) {}

    void execute(Environment &env) const override
    {
        Functions::define(name, std::make_unique<UserDefineFunction>(type, arg_types, arg_names, arg_defaults, body));
    }
};
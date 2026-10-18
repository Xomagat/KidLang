//
// Created by Xomagat on 15.09.2026.
//

#ifndef KID_PARSER_H
#define KID_PARSER_H

#pragma once
#include <memory>
#include <vector>
#include <unordered_set>

#include "Token.h"
#include "TokenType.h"

#include "AST/BinExpression.h"
#include "AST/ValueExpression.h"
#include "AST/UnaryExpression.h"
#include "AST/ConditionalExpression.h"
#include "AST/VariablesExpression.h"
#include "AST/FunctionalExpression.h"
#include "AST/ConvertExpression.h"
#include "AST/MethodExpression.h"
#include "AST/ObjectExpression.h"
#include "AST/ListExpression.h"
#include "AST/Expression.h"

#include "AST/AssignmentStatement.h"
#include "AST/LoopStatement.h"
#include "AST/BreakStatement.h"
#include "AST/ContinueStatement.h"
#include "AST/ReturnStatement.h"
#include "AST/FunctionStatement.h"
#include "AST/ClassesStatemant.h"
#include "AST/IOStatement.h"
#include "AST/IfStatement.h"
#include "AST/ImportStatement.h"
#include "AST/Statement.h"

struct FuncParts
{
    std::string type, name;
    std::vector<std::string> arg_types, arg_names;
    std::vector<std::shared_ptr<Expression>> arg_defaults;
    std::unique_ptr<Statement> body;
};

class Parser
{
private:
    Token eof;
    std::vector<Token> tokens;

    int pos;
    int size;

    Token get(int relative_position);

    std::unique_ptr<Statement> block();

    std::unique_ptr<Statement> statement();
    std::string parse_type();
    std::unique_ptr<Statement> assigment_statement();
    std::unique_ptr<Statement> if_else();
    std::unique_ptr<Statement> repeat_statement();
    std::unique_ptr<Statement> while_statement();
    std::unique_ptr<Statement> for_statement();
    std::unique_ptr<Statement> define_function();
    std::unique_ptr<Statement> index_assignment();

    std::unique_ptr<Expression> function();
    std::unique_ptr<Expression> expression();
    std::unique_ptr<Expression> logic_or();
    std::unique_ptr<Expression> logic_and();
    std::unique_ptr<Expression> logic_not();

    std::unique_ptr<Expression> equality();

    std::unique_ptr<Expression> conditional();

    std::unique_ptr<Expression> additive();
    std::unique_ptr<Expression> multiply();
    std::unique_ptr<Expression> pow();
    std::unique_ptr<Expression> factorials();
    std::unique_ptr<Expression> postfix();
    std::unique_ptr<Expression> unary();
    std::unique_ptr<Expression> primary();

    bool match(token_type type);

    std::string at();
    Token consume(token_type type);
    Token consume(token_type type, const std::string& hint);

    std::unordered_set<std::string> known_classes;
    bool is_type_start(Token t);
    std::unique_ptr<Expression> default_expr(const std::string& type);
    std::unique_ptr<Statement> class_definition();
    std::unique_ptr<Statement> member_statement();

    FuncParts function_parts();

public:
    Parser(std::vector<Token> tokens);

    std::vector<std::unique_ptr<Statement>> parse();
};

#endif
//
// Created by Xomagat on 15.09.2026.
//

#ifndef KID_PARSER_H
#define KID_PARSER_H

#pragma once
#include <memory>
#include <vector>

#include "Token.h"
#include "TokenType.h"

#include "AST/BinExpression.h"
#include "AST/ValueExpression.h"
#include "AST/UnaryExpression.h"
#include "AST/ConditionalExpression.h"
#include "AST/VariablesExpression.h"
#include "AST/FunctionalExpression.h"
#include "AST/Expression.h"

#include "AST/AssignmentStatement.h"
#include "AST/LoopStatement.h"
#include "AST/BreakStatement.h"
#include "AST/ContinueStatement.h"
#include "AST/ReturnStatement.h"
#include "AST/FunctionStatement.h"
#include "AST/IOStatement.h"
#include "AST/IfStatement.h"
#include "AST/Statement.h"

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
    std::unique_ptr<Statement> assigment_statement();
    std::unique_ptr<Statement> if_else();
    std::unique_ptr<Statement> repeat_statement();
    std::unique_ptr<Statement> while_statement();
    std::unique_ptr<Statement> define_function();

    std::unique_ptr<Expression> function();
    std::unique_ptr<Expression> expression();

    std::unique_ptr<Expression> equality();

    std::unique_ptr<Expression> conditional();

    std::unique_ptr<Expression> additive();
    std::unique_ptr<Expression> multiply();
    std::unique_ptr<Expression> pow();
    std::unique_ptr<Expression> unary();
    std::unique_ptr<Expression> primary();

    bool match(token_type type);

    Token consume(token_type type);

public:
    Parser(std::vector<Token> tokens);

    std::vector<std::unique_ptr<Statement>> parse();
};

#endif // KID_PARSER_H

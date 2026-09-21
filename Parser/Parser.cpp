//
// Created by Xomagat on 15.09.2026.
//

#include "Parser.h"

// vars

// funcs
Parser::Parser(std::vector<Token> tokens)
{
    eof = Token(token_type::eof, "");

    this->tokens = tokens;

    size = tokens.size();

    pos = 0;
}

std::vector<std::unique_ptr<Statement>> Parser::parse()
{
    std::vector<std::unique_ptr<Statement>> result;

    while (!match(token_type::eof))
    {
        result.push_back(statement());
    }

    return result;
}

std::unique_ptr<Statement> Parser::block()
{
    std::vector<std::unique_ptr<Statement>> statements;

    consume(token_type::INDENT);

    while (!match(token_type::DEDENT) && !match(token_type::eof))
        statements.push_back(statement());

    return std::make_unique<BlockStatement>(std::move(statements));
}

std::unique_ptr<Statement> Parser::statement()
{
    switch (get(0).get_type())
    {
        case token_type::SHOW: {
            consume(token_type::SHOW);
            consume(token_type::LPARENT);
            std::unique_ptr<Expression> expr = expression();
            consume(token_type::RPARENT);
            return std::make_unique<ShowStatement>(std::move(expr));
        }
        case token_type::IF: {
            consume(token_type::IF);
            return if_else();
        }
        case token_type::REPEAT: {
            consume(token_type::REPEAT);
            return repeat_statement();
        }
        case token_type::BREAK: {
            consume(token_type::BREAK);
            return std::make_unique<BreakStatement>();
        }
        case token_type::CONTINUE: {
            consume(token_type::CONTINUE);
            return std::make_unique<ContinueStatement>();
        }

        case token_type::NUMB:
        case token_type::STRING:
        case token_type::LOGIC:
            return assigment_statement();

        case token_type::WORDS:
            return assigment_statement();

        default: {
            throw std::runtime_error("Неопределенный токен: " + token_string[get(0).get_type()]);
        }
    }
}

bool type_check(token_type type)
{
    switch (type)
    {
        case token_type::NUMB:
        case token_type::STRING:
        case token_type::LOGIC:
            return true;
    }

    return false;
}

std::unique_ptr<Statement> Parser::assigment_statement()
{
    // type name = 33 or type name
    Token current = get(0);

    if (type_check(current.get_type()) && get(1).get_type() == token_type::WORDS)
    {
        std::string type = consume(current.get_type()).get_text();
        std::string name = consume(token_type::WORDS).get_text();
        std::unique_ptr<Expression> expr;

        if (match(token_type::EQ))
            expr = expression();
        else
            expr = std::make_unique<ValueExpression>(nullptr);

        return std::make_unique<AssignmentStatement>(type, name, std::move(expr));
    }
    else if (current.get_type() == token_type::WORDS && get(1).get_type() == token_type::EQ)
    {
        std::string name = consume(token_type::WORDS).get_text();
        consume(token_type::EQ);

        std::unique_ptr<Expression> expr = expression();

        return std::make_unique<AssignmentStatement>("", name, std::move(expr));
    }

    throw std::runtime_error("При объявлении переменной, было пропущено имя или тип!");
}

std::unique_ptr<Statement> Parser::if_else()
{
    std::vector<std::pair<std::unique_ptr<Expression>, std::unique_ptr<Statement>>> branches;

    std::unique_ptr<Expression> cond = expression();
    branches.emplace_back(std::move(cond), block());

    while (match(token_type::ELIF))
    {
        std::unique_ptr<Expression> elif_cond = expression();
        branches.emplace_back(std::move(elif_cond), block());
    }

    std::unique_ptr<Statement> else_body;
    if (match(token_type::ELSE))
        else_body = block();

    return std::make_unique<IfStatement>(std::move(branches), std::move(else_body));
}

std::unique_ptr<Statement> Parser::repeat_statement()
{
    double counter = std::stod(consume(token_type::NUMBER).get_text());

    consume(token_type::ONCE);
    std::unique_ptr<Statement> body = block();

    return std::make_unique<RepeatStatement>(counter, std::move(body));
}

std::unique_ptr<Expression> Parser::expression()
{
    return equality();
}

std::unique_ptr<Expression> Parser::equality()
{
    std::unique_ptr<Expression> expr = conditional();

    while (true)
    {
        if (match(token_type::EQEQ))
        {
            expr = std::make_unique<ConditionalExpression>("==", std::move(expr), conditional());
            continue;
        }
        if (match(token_type::NOEQ))
        {
            expr = std::make_unique<ConditionalExpression>("!=", std::move(expr), conditional());
            continue;
        }
        break;
    }

    return expr;
}

std::unique_ptr<Expression> Parser::conditional()
{
    std::unique_ptr<Expression> expr = additive();



    while (true)
    {
        if (match(token_type::GTEQ))
        {
            expr = std::make_unique<ConditionalExpression>(">=", std::move(expr), additive());
            continue;
        }
        if (match(token_type::LTEQ))
        {
            expr = std::make_unique<ConditionalExpression>("<=", std::move(expr), additive());
            continue;
        }
        if (match(token_type::GT))
        {
            expr = std::make_unique<ConditionalExpression>(">", std::move(expr), additive());
            continue;
        }
        if (match(token_type::LT))
        {
            expr = std::make_unique<ConditionalExpression>("<", std::move(expr), additive());
            continue;
        }
        break;
    }


    return expr;
}

std::unique_ptr<Expression> Parser::additive()
{

    std::unique_ptr<Expression> expr = multiply();

    while (true)
    {
        if (match(token_type::PLUS))
        {
            expr = std::make_unique<BinExpression>('+', std::move(expr), multiply());
            continue;
        }
        if (match(token_type::MINUS))
        {
            expr = std::make_unique<BinExpression>('-', std::move(expr), multiply());
            continue;
        }
        break;
    }


    return expr;
}

std::unique_ptr<Expression> Parser::multiply()
{
    std::unique_ptr<Expression> expr = pow();

    while (true)
    {
        if (match(token_type::MULT))
        {
            expr = std::make_unique<BinExpression>('*', std::move(expr), pow());
            continue;
        }
        if (match(token_type::DIV))
        {
            expr = std::make_unique<BinExpression>('/', std::move(expr), pow());
            continue;
        }
        break;
    }


    return expr;
}

std::unique_ptr<Expression> Parser::pow()
{
    std::unique_ptr<Expression> expr = unary();

    while (true)
    {
        if (match(token_type::POW))
        {
            expr = std::make_unique<BinExpression>('p', std::move(expr), unary());
            continue;
        }
        break;
    }


    return expr;
}

std::unique_ptr<Expression> Parser::unary()
{
    if (match(token_type::MINUS))
        return std::make_unique<UnaryExpression>('-', std::move(primary()));
    if (match(token_type::PLUS))
        return std::make_unique<UnaryExpression>('+', std::move(primary()));

    return primary();
}

std::unique_ptr<Expression> Parser::primary()
{
    Token current = get(0);

    if (match(token_type::NUMBER))
    {
        return std::make_unique<ValueExpression>(std::stod(current.get_text()));
    }
    if (match(token_type::TEXT))
        return std::make_unique<ValueExpression>(current.get_text());
    if (match(token_type::WORDS))
        return std::make_unique<VariableExpression>(current.get_text());
    if (match(token_type::LPARENT))
    {
        std::unique_ptr<Expression> result = expression();
        match(token_type::RPARENT);
        return result;
    }

    throw std::runtime_error("Неизвестное выражение! " + current.get_text());
}

Token Parser::get(int relative_position)
{
    int position = pos + relative_position;
    if (position >= size)
        return eof;

    return tokens[position];
}

bool Parser::match(token_type type)
{
    Token t = get(0);

    if (type != t.get_type())
        return false;

    pos++;
    return true;
}

Token Parser::consume(token_type type)
{
    Token t = get(0);

    if (type != t.get_type())
        throw std::runtime_error("Слово " + token_string[t.get_type()] + " не совпадает c " + token_string[type]);

    pos++;
    return t;
}
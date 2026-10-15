//
// Created by Xomagat on 15.09.2026.
//

#include "Token.h"

// vars

// funcs
Token::Token()
{
    this->type = token_type::eof;
    this->text = "";
    this->line = 0;
}

Token::Token(token_type type, std::string text, int line)
{
    this->text = text;
    this->type = type;
    this->line = line;
}
int Token::get_line() { return line; }

std::string Token::get_text()
{
    return this->text;
}
token_type Token::get_type()
{
    return this->type;
}

void Token::set_text(std::string text)
{
    this->text = text;
}
void Token::set_type(token_type type)
{
    this->type = type;
}
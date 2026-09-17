//
// Created by Xomagat on 15.09.2026.
//

#ifndef KID_TOKEN_H
#define KID_TOKEN_H

#include <string>

#include "TokenType.h"

class Token
{
private:
    std::string text;   // value
    token_type type;     // type

public:
    // constructors
    Token();
    Token(token_type type, std::string text);

    // getters
    std::string get_text();
    token_type get_type();

    // setters
    void set_text(std::string text);
    void set_type(token_type type);
};

#endif // KID_TOKEN_H

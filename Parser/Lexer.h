//
// Created by Xomagat on 15.09.2026.
//

#ifndef KID_LEXER_H
#define KID_LEXER_H

#pragma once
#include <string>
#include <stdexcept>
#include <vector>
#include <unordered_map>

#include "Token.h"
#include "TokenType.h"

class Lexer
{
private:
    std::vector<int> indent_stack{0};

    std::u32string OPERATION_CHARS;
    std::unordered_map<std::u32string, token_type> SINGLE_OPERATORS;

    std::u32string code;        // декодированный код (code points, не байты)
    std::vector<Token> tokens;  // tokens

    int pos;                    // position now
    int length;                 // code length (в code points)
    int line = 1;               // line now

    // utf-8 helpers
    static std::u32string decode_utf8(const std::string& input);
    static std::string encode_utf8(const std::u32string& input);
    static bool is_letter(char32_t c);
    static bool is_digit(char32_t c);

    // helpers func
    void add_token(token_type type);
    void add_token(token_type type, const std::u32string& text);

    char32_t peek(int relative_position);
    char32_t next();

    void tokenize_number();
    void tokenize_string();
    void tokenize_operation();
    void tokenize_word();
    void handle_indentation();

public:
    explicit Lexer(const std::string& code);

    std::vector<Token> tokenize();
};


#endif // KID_LEXER_H
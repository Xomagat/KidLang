//
// Created by Xomagat on 15.09.2026.
//

#ifndef KID_TOKENTYPE_H
#define KID_TOKENTYPE_H

#include <unordered_map>

enum token_type
{
    // types
    NUMBER,
    WORDS,
    TEXT,

    // keywords
    SHOW,
    NUMB,
    STRING,
    LOGIC,
    TRUE,
    FALSE,

    // op
    PLUS,
    MINUS,
    MULT,
    DIV,
    POW,
    EQ,
    LPARENT,
    RPARENT,

    eof
};

inline std::unordered_map<token_type, std::string> token_string = {
    {NUMBER, "цифорка"}, {WORDS, "слово"}, {TEXT, "текст"},
    {SHOW, "покажи"}, {NUMB, "число"}, {STRING, "текст"},
    {LOGIC, "ответ"}, {TRUE, "да"}, {FALSE, "нет"},
    {PLUS, "+"}, {MINUS, "-"}, {MULT, "*"}, {DIV, "/"}, {POW, "**"}, {EQ, "="},
    {LPARENT, "("}, {RPARENT, ")"},
    {eof, "<конец файла>"}
};

#endif // KID_TOKENTYPE_H

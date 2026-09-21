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
    IF,
    ELIF,
    ELSE,
    REPEAT,
    ONCE,
    WHILE,
    BREAK,
    CONTINUE,
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
    LT,
    GT,
    NOT,
    EQEQ,
    NOEQ,
    LTEQ,
    GTEQ,
    LPARENT,
    RPARENT,

    INDENT,
    DEDENT,

    eof
};

inline std::unordered_map<token_type, std::string> token_string = {
    {NUMBER, "цифорка"}, {WORDS, "слово"}, {TEXT, "текст"},
    {SHOW, "покажи"}, {IF, "если"}, {ELIF, "может"},
    {REPEAT, "повтори"}, {ONCE, "раз/раза"}, {WHILE, "пока"},
    {BREAK, "стоп"}, {CONTINUE, "продолжи"},
    {ELSE, "иначе"}, {NUMB, "число"}, {STRING, "текст"},
    {LOGIC, "ответ"}, {TRUE, "да"}, {FALSE, "нет"},
    {PLUS, "+"}, {MINUS, "-"}, {MULT, "*"}, {DIV, "/"}, {POW, "**"}, {EQ, "="},
    {EQEQ, "=="}, {NOEQ, "!="}, {GT, ">"}, {LT, "<"}, {GTEQ, ">="}, {LTEQ, "<="},
    {LPARENT, "("}, {RPARENT, ")"}, {NOT, "!"},
    {INDENT, "<отступ>"}, {DEDENT, "<конец отступа>"},
    {eof, "<конец файла>"}
};

#endif // KID_TOKENTYPE_H
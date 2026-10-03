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
    GET,
    IF,
    ELIF,
    ELSE,
    REPEAT,
    ONCE,
    WHILE,
    BREAK,
    CONTINUE,
    DEFINE,
    RETURN,
    RETURNED,
    FOR,
    FROM,
    TO,
    ADDTO,
    NUMB,
    STRING,
    LOGIC,
    NOTHING,
    TRUE,
    FALSE,
    AND,
    OR,
    NOT,

    // op
    PLUS,
    MINUS,
    MULT,
    DIV,
    POW,
    MOD,
    EQ,
    LT,
    GT,
    EM,
    EQEQ,
    NOEQ,
    LTEQ,
    GTEQ,
    LPARENT,
    RPARENT,
    COMMA,

    INDENT,
    DEDENT,

    eof
};

inline std::unordered_map<token_type, std::string> token_string = {
    {NUMBER, "цифорка"}, {WORDS, "слово"}, {TEXT, "текст"},
    {SHOW, "покажи"}, {IF, "если"}, {ELIF, "может"},
    {REPEAT, "повтори"}, {ONCE, "раз/раза"}, {WHILE, "пока"},
    {BREAK, "стоп"}, {CONTINUE, "продолжи"}, {DEFINE, "рецепт"}, {RETURN, "верни"},
    {RETURNED, "вернет/вернёт"}, {NOTHING, "ничего"}, {AND, "и"}, {OR, "или"},
    {NOT, "не"},
    {GET, "получи"}, {FOR, "для"}, {FROM, "от"}, {TO, "до"},
    {ELSE, "иначе"}, {NUMB, "число"}, {STRING, "текст"},
    {LOGIC, "ответ"}, {TRUE, "да"}, {FALSE, "нет"},
    {PLUS, "+"}, {MINUS, "-"}, {MULT, "*"}, {DIV, "/"}, {POW, "**"}, {EQ, "="},
    {EQEQ, "=="}, {NOEQ, "!="}, {GT, ">"}, {LT, "<"}, {GTEQ, ">="}, {LTEQ, "<="},
    {LPARENT, "("}, {RPARENT, ")"}, {EM, "!"},
    {INDENT, "<отступ>"}, {DEDENT, "<конец отступа>"},
    {eof, "<конец файла>"}
};

#endif // KID_TOKENTYPE_H
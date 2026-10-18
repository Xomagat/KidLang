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
    CONVERT,
    IN,
    LIST,
    OF,
    DICT,
    CLASS,

    // op
    PLUS,
    MINUS,
    MULT,
    DIV,
    INTDIV,
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
    LBRACKET,
    RBRACKET,
    LBRACE,
    RBRACE,
    COMMA,
    DOT,
    COLON,

    PLUSEQ,
    MINUSEQ,
    MULTEQ,
    DIVEQ,
    MODEQ,
    POWEQ,
    INTDIVEQ,

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
    {NOT, "не"}, {CONVERT, "преврати"}, {IN, "в"}, {LIST, "список"}, {OF, "из"},
    {GET, "получи"}, {FOR, "для"}, {FROM, "от"}, {TO, "до"},
    {DICT, "словарь"}, {CLASS, "класс"}, {LBRACE, "{"}, {RBRACE, "}"}, {COLON, ":"},
    {ELSE, "иначе"}, {NUMB, "число"}, {STRING, "текст"},
    {ADDTO, "добавляя"}, {MOD, "%"}, {MODEQ, "%="},
    {LOGIC, "условие"}, {TRUE, "верно"}, {FALSE, "неверно"}, {COMMA, ","}, {DOT, "."},
    {PLUS, "+"}, {MINUS, "-"}, {MULT, "*"}, {DIV, "/"}, {POW, "**"}, {EQ, "="},
    {INTDIV, "//"}, {PLUSEQ, "+="}, {MINUSEQ, "-="}, {MULTEQ, "*="}, {DIVEQ, "/="}, {POWEQ, "**="},
    {INTDIVEQ, "//="}, {LBRACKET, "["}, {RBRACKET, "]"},
    {EQEQ, "=="}, {NOEQ, "!="}, {GT, ">"}, {LT, "<"}, {GTEQ, ">="}, {LTEQ, "<="},
    {LPARENT, "("}, {RPARENT, ")"}, {EM, "!"},
    {INDENT, "отступ"}, {DEDENT, "конец отступа"},
    {eof, "конец файла"}
};

inline std::string describe(token_type t)
{
    auto it = token_string.find(t);
    return it == token_string.end() ? "?" : it->second;
}

#endif // KID_TOKENTYPE_H
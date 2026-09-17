//
// Created by Xomagat on 15.09.2026.
//

#include "Lexer.h"

// vars

// funcs
Lexer::Lexer(const std::string& source)
{
    OPERATION_CHARS = U"+-*/=()";
    SINGLE_OPERATORS = {
        {U'+', token_type::PLUS},
        {U'-', token_type::MINUS},
        {U'*', token_type::MULT},
        {U'/', token_type::DIV},
        {U'=', token_type::EQ},
        {U'(', token_type::LPARENT},
        {U')', token_type::RPARENT},
    };

    code = decode_utf8(source);   // тут и происходит настоящее декодирование UTF-8

    length = static_cast<int>(code.size());
    pos = 0;

    tokens = std::vector<Token>();
}

std::vector<Token> Lexer::tokenize()
{
    while (pos < length)
    {
        char32_t current = peek(0);

        if (current == U'\n' || current == U' ')
        {
            line++;
            next();
            continue;
        }
        if (current == U' ')
        {
            next();
            continue;
        }

        if (current == U'\r')
        {
            next();
            continue;
        }

        if (current == U'"')
            tokenize_string();
        else if (is_digit(current))
            tokenize_number();
        else if (is_letter(current))
            tokenize_word();
        else if (OPERATION_CHARS.find(current) != std::u32string::npos)
            tokenize_operation();
    }

    return tokens;
}

void Lexer::tokenize_number()
{
    std::u32string buffer;
    char32_t current = peek(0);

    while (true)
    {
        if (current == U'.')
        {
            if (buffer.find(U'.') != std::u32string::npos)
                throw std::runtime_error("Ой, кажется, в строчке: " + std::to_string(line)
                    + " не правильно написанно число! Я не могу его запомнить.");
        }
        else if (!is_digit(current))
            break;
        buffer.push_back(current);
        current = next();
    }

    add_token(token_type::NUMBER, buffer);
}

void Lexer::tokenize_string()
{
    next(); // skip "
    std::u32string buffer;
    char32_t current = peek(0);

    while (current != '"' && current != '\0')
    {
        if (current == '\\')
        {
            current = next();

            switch (current)
            {
            case '"': current = next(); buffer.push_back('"'); continue;
            case 'n': current = next(); buffer.push_back('\n'); continue;
            case 'r': current = next(); buffer.push_back('\r'); continue;
            case 't': current = next(); buffer.push_back('\t'); continue;
            }

            buffer.push_back('\\');
            continue;
        }

        buffer.push_back(current);
        current = next();
    }
    next(); // skip "

    add_token(token_type::TEXT, buffer);
}

void Lexer::tokenize_operation()
{
    char32_t one = peek(0);
    char32_t two = peek(1);

    // "**" — единственный двухсимвольный оператор
    if (one == U'*' && two == U'*')
    {
        add_token(token_type::POW);
        next(); next();
        return;
    }

    auto it = SINGLE_OPERATORS.find(one);
    if (it != SINGLE_OPERATORS.end())
    {
        add_token(it->second);
        next();
        return;
    }

    throw std::runtime_error("Ой, я не знаю: " + encode_utf8(std::u32string(1, one)) + "! Ты уверен, что написал правильно?");
}

void Lexer::tokenize_word()
{
    std::u32string buffer;
    char32_t current = peek(0);

    while (true)
    {
        if (!is_letter(current) && !is_digit(current) && current != U'_')
            break;
        buffer.push_back(current);
        current = next();
    }

    static const std::unordered_map<std::u32string, token_type> keywords = {
        {U"покажи", token_type::SHOW},
        {U"число",  token_type::NUMB},
        {U"текст",  token_type::STRING},
        {U"ответ",  token_type::LOGIC},
        {U"да",     token_type::TRUE},
        {U"нет",    token_type::FALSE},
    };

    auto it = keywords.find(buffer);
    if (it != keywords.end())
    {
        add_token(it->second, buffer);
    }
    else
    {
        add_token(token_type::WORDS, buffer);
    }
}

void Lexer::add_token(token_type type)
{
    add_token(type, U"");
}

void Lexer::add_token(token_type type, const std::u32string& text)
{
    tokens.emplace_back(type, encode_utf8(text));
}

char32_t Lexer::next()
{
    pos++;
    return peek(0);
}

char32_t Lexer::peek(int relative_position)
{
    int position = pos + relative_position;
    if (position >= length || position < 0)
        return U'\0';

    return code[position];
}

// --- UTF-8 <-> char32_t ---

std::u32string Lexer::decode_utf8(const std::string& input)
{
    std::u32string result;
    size_t i = 0;
    size_t n = input.size();

    while (i < n)
    {
        unsigned char c = static_cast<unsigned char>(input[i]);
        char32_t cp;
        int len;

        if (c < 0x80)
        {
            cp = c;
            len = 1;
        }
        else if ((c & 0xE0) == 0xC0)
        {
            cp = c & 0x1F;
            len = 2;
        }
        else if ((c & 0xF0) == 0xE0)
        {
            cp = c & 0x0F;
            len = 3;
        }
        else if ((c & 0xF8) == 0xF0)
        {
            cp = c & 0x07;
            len = 4;
        }
        else
        {
            throw std::runtime_error("Некорректная UTF-8 последовательность в коде!");
        }

        if (i + len > n)
            throw std::runtime_error("Оборванная UTF-8 последовательность в коде!");

        for (int k = 1; k < len; k++)
        {
            unsigned char cc = static_cast<unsigned char>(input[i + k]);
            if ((cc & 0xC0) != 0x80)
                throw std::runtime_error("Некорректная UTF-8 последовательность в коде!");
            cp = (cp << 6) | (cc & 0x3F);
        }

        result.push_back(cp);
        i += len;
    }

    return result;
}

std::string Lexer::encode_utf8(const std::u32string& input)
{
    std::string result;

    for (char32_t cp : input)
    {
        if (cp <= 0x7F)
        {
            result.push_back(static_cast<char>(cp));
        }
        else if (cp <= 0x7FF)
        {
            result.push_back(static_cast<char>(0xC0 | (cp >> 6)));
            result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
        else if (cp <= 0xFFFF)
        {
            result.push_back(static_cast<char>(0xE0 | (cp >> 12)));
            result.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
        else
        {
            result.push_back(static_cast<char>(0xF0 | (cp >> 18)));
            result.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
    }

    return result;
}

bool Lexer::is_letter(char32_t c)
{
    return (c >= U'a' && c <= U'z') ||
           (c >= U'A' && c <= U'Z') ||
           (c >= U'а' && c <= U'я') ||
           (c >= U'А' && c <= U'Я') ||
           c == U'ё' || c == U'Ё';
}

bool Lexer::is_digit(char32_t c)
{
    return c >= U'0' && c <= U'9';
}
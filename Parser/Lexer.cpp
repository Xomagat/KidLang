//
// Created by Xomagat on 15.09.2026.
//

#include "Lexer.h"

#include <algorithm>

// vars

// funcs
Lexer::Lexer(const std::string& source)
{
    OPERATION_CHARS = U"+-*%/=!()[]{}<>,.:";
    SINGLE_OPERATORS = {
        {U"+",  token_type::PLUS},
        {U"-",  token_type::MINUS},
        {U"*",  token_type::MULT},
        {U"/",  token_type::DIV},
        {U"//", token_type::INTDIV},
        {U"**", token_type::POW},
        {U"%",  token_type::MOD},
        {U"=",  token_type::EQ},
        {U"+=", token_type::PLUSEQ},
        {U"-=", token_type::MINUSEQ},
        {U"*=", token_type::MULTEQ},
        {U"/=", token_type::DIVEQ},
        {U"//=",token_type::INTDIVEQ},
        {U"**=",token_type::POWEQ},
        {U"%=", token_type::MODEQ},
        {U"!",  token_type::EM},
        {U"(",  token_type::LPARENT},
        {U")",  token_type::RPARENT},
        {U"[",  token_type::LBRACKET},
        {U"]",  token_type::RBRACKET},
        {U"{",  token_type::LBRACE},
        {U"}",  token_type::RBRACE},
        {U">",  token_type::GT},
        {U"<",  token_type::LT},
        {U"==", token_type::EQEQ},
        {U"!=", token_type::NOEQ},
        {U">=", token_type::GTEQ},
        {U"<=", token_type::LTEQ},
        {U",",  token_type::COMMA},
        {U".",  token_type::DOT},
        {U":",  token_type::COLON},
    };

    code = decode_utf8(source);

    length = static_cast<int>(code.size());
    pos = 0;

    tokens = std::vector<Token>();
}

std::vector<Token> Lexer::tokenize()
{
    handle_indentation();

    while (pos < length)
    {
        char32_t current = peek(0);

        if (current == U' ' || current == U'\t')
        {
            next();
            continue;
        }

        if (current == U'\r')
        {
            next();
            continue;
        }

        if (current == U'\n')
        {
            next(); line++;
            if (depth == 0) handle_indentation();
            continue;
        }

        if (current == U';')
        {
            while (pos < length && peek(0) != U'\n')
                next();
            continue;
        }

        if (current == U'"' || current == U'\'')
            tokenize_string();
        else if (is_digit(current))
            tokenize_number();
        else if (is_letter(current))
            tokenize_word();
        else if (OPERATION_CHARS.find(current) != std::u32string::npos)
            tokenize_operation();
        else
            throw std::runtime_error("Я не знаю, что такое:" + std::to_string(current) + "!");
    }

    while (indent_stack.size() > 1)
    {
        indent_stack.pop_back();
        add_token(token_type::DEDENT);
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
            if (!is_digit(peek(1))) break;
            if (buffer.find(U'.') != std::u32string::npos)
                throw std::runtime_error("В числе две точки. Так писать нельзя, я не пойму, какое это число!");
        }
        else if (!is_digit(current))
            break;
        buffer.push_back(current);
        current = next();
    }

    add_token(token_type::NUMBER, buffer);
}

void Lexer::handle_indentation()
{
    while (true)
    {
        int indent = 0;

        while (peek(0) == U' ' || peek(0) == U'\t')
        {
            indent += (peek(0) == U'\t') ? 4 : 1;
            next();
        }

        if (peek(0) == U';')
        {
            while (pos < length && peek(0) != U'\n')
                next();
            continue;
        }

        if (peek(0) == U'\r')
        {
            next();
            continue;
        }
        if (peek(0) == U'\n')
        {
            next();
            line++;
            continue;
        }
        if (pos >= length)
            return;

        if (indent > indent_stack.back())
        {
            indent_stack.push_back(indent);
            add_token(token_type::INDENT);
        }
        else
        {
            while (indent < indent_stack.back())
            {
                indent_stack.pop_back();
                add_token(token_type::DEDENT);
            }

            if (indent != indent_stack.back())
                throw std::runtime_error("Этот отступ не такой, как у строк выше. Проверь пробелы в начале строки!");
        }

        return;
    }
}

void Lexer::tokenize_string()
{
    next(); // skip "
    std::u32string buffer;
    char32_t current = peek(0);

    while (current != '"' || current != '\'' && current != '\0')
    {
        if (current == '\\')
        {
            current = next();

            switch (current)
            {
            case '"': current = next();  buffer.push_back('"');  continue;
            case '\'': current = next(); buffer.push_back('\''); continue;
            case 'n': current = next();  buffer.push_back('\n'); continue;
            case 'r': current = next();  buffer.push_back('\r'); continue;
            case 't': current = next();  buffer.push_back('\t'); continue;
            }

            buffer.push_back('\\');
            continue;
        }

        buffer.push_back(current);
        current = next();
    }

    if (current != U'"')
        throw std::runtime_error("Ты открыл кавычку, а закрыть забыл!");

    next(); // skip "

    add_token(token_type::TEXT, buffer);
}

void Lexer::tokenize_operation()
{
    for (int len = 3; len >= 1; len--)
    {
        std::u32string op;
        for (int i = 0; i < len && peek(i) != U'\0'; i++)
            op.push_back(peek(i));

        if ((int)op.size() != len) continue;

        auto it = SINGLE_OPERATORS.find(op);
        if (it != SINGLE_OPERATORS.end())
        {
            switch (it->second) {
                case token_type::LPARENT: case token_type::LBRACKET: case token_type::LBRACE: depth++; break;
                case token_type::RPARENT: case token_type::RBRACKET: case token_type::RBRACE: if (depth > 0) depth--; break;
                default: break;
            }
            add_token(it->second);
            for (int i = 0; i < len; i++) next();
            return;
        }
    }

    throw std::runtime_error("Я не знаю знак «"
    + encode_utf8(std::u32string(1, peek(0))) + "». Ты уверен, что написал правильно?");
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
        {U"покажи",   token_type::SHOW},
        {U"получи",   token_type::GET},
        {U"если",     token_type::IF},
        {U"может",    token_type::ELIF},
        {U"иначе",    token_type::ELSE},
        {U"повтори",  token_type::REPEAT},
        {U"раз",      token_type::ONCE},
        {U"раза",     token_type::ONCE},
        {U"пока",     token_type::WHILE},
        {U"стоп",     token_type::BREAK},
        {U"рецепт",   token_type::DEFINE},
        {U"верни",    token_type::RETURN},
        {U"вернет",   token_type::RETURNED},
        {U"вернёт",   token_type::RETURNED},
        {U"ничего",   token_type::NOTHING},
        {U"для",      token_type::FOR},
        {U"от",       token_type::FROM},
        {U"до",       token_type::TO},
        {U"добавляя", token_type::ADDTO},
        {U"продолжи", token_type::CONTINUE},
        {U"число",    token_type::NUMB},
        {U"текст",    token_type::STRING},
        {U"условие",  token_type::LOGIC},
        {U"верно",    token_type::TRUE},
        {U"неверно",  token_type::FALSE},
        {U"и",        token_type::AND},
        {U"или",      token_type::OR},
        {U"не",       token_type::NOT},
        {U"преврати", token_type::CONVERT},
        {U"в",        token_type::IN},
        {U"список", token_type::LIST},
        {U"из",     token_type::OF},
        {U"словарь", token_type::DICT},
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
    tokens.emplace_back(type, encode_utf8(text), line);
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
            throw std::runtime_error("В файле есть странный символ! Прости но я не знаю, что он обозначает. :(");
        }

        if (i + len > n)
            throw std::runtime_error("Кажется код оборвался!");

        for (int k = 1; k < len; k++)
        {
            unsigned char cc = static_cast<unsigned char>(input[i + k]);
            if ((cc & 0xC0) != 0x80)
                throw std::runtime_error("В файле есть странный символ! Прости но я не знаю, что он обозначает. :(");
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
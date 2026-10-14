//
// Created by Xomagat on 14.10.2026.
//

#ifndef KID_CONSOLE_H
#define KID_CONSOLE_H

#pragma once
#include <cstdlib>
#include <iostream>
#include <string>

#if defined(_WIN32)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>

    // windows.h определяет эти макросы, а у нас они — элементы token_type
    #undef TRUE
    #undef FALSE
    #undef IN
    #undef OUT

    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
        #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
#else
    #include <unistd.h>
#endif

enum class Color
{
    Default      = 0,
    Black        = 30,
    Red          = 31,
    Green        = 32,
    Yellow       = 33,
    Blue         = 34,
    Magenta      = 35,
    Cyan         = 36,
    White        = 37,
    Gray         = 90,
    BrightRed    = 91,
    BrightGreen  = 92,
    BrightYellow = 93,
    BrightBlue   = 94,
    BrightCyan   = 96,
};

class Console
{
private:
    static bool detect(bool is_err)
    {
        if (std::getenv("NO_COLOR")) return false;

#if defined(_WIN32)
        HANDLE h = GetStdHandle(is_err ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);
        if (h == nullptr || h == INVALID_HANDLE_VALUE) return false;

        // GetConsoleMode работает только для настоящей консоли (не файл/pipe)
        DWORD mode = 0;
        if (!GetConsoleMode(h, &mode)) return false;
        if (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) return true;

        return SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
#else
        if (!isatty(is_err ? STDERR_FILENO : STDOUT_FILENO)) return false;

        const char* term = std::getenv("TERM");
        return term && std::string(term) != "dumb";
#endif
    }

public:
    static bool colors_enabled(bool is_err = false)
    {
        static const bool out = detect(false);
        static const bool err = detect(true);
        return is_err ? err : out;
    }

    static std::string paint(const std::string& text, Color color,
                             bool bold = false, bool is_err = false)
    {
        if (!colors_enabled(is_err)) return text;

        std::string codes;
        if (bold) codes += "1;";
        codes += std::to_string(static_cast<int>(color));

        return "\033[" + codes + "m" + text + "\033[0m";
    }

    static void print(const std::string& text, Color color,
                      bool bold = false, std::ostream& os = std::cout)
    {
        os << paint(text, color, bold, &os == &std::cerr);
    }

    static size_t utf8_len(const std::string& s)
    {
        size_t n = 0;
        for (unsigned char c : s)
            if ((c & 0xC0) != 0x80) n++;
        return n;
    }
};

#endif // KID_CONSOLE_H
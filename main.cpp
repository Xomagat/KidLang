#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <filesystem>

#include "Parser/Lexer.h"
#include "Parser/Parser.h"

#include "libs/Environment.h"

#if _WIN32
#include <windows.h>
#endif

#define VERSION "0.1"

int main(int argc, char* argv[])
{
#if _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif

    if (argc > 1)
    {
        if (std::strcmp(argv[1], "-v") == 0)
        {
            std::cout << "KidLang version: " << VERSION << std::endl;
            return 0;
        }

        std::ifstream file(argv[1]);
        std::stringstream code;

        if (std::filesystem::directory_entry(argv[1]).path().extension() != ".kl")
            std::cerr << "Ошибка! Я могу читать только мой файл .kl" << std::endl;

        if (!file.is_open())
            std::cerr << "Я не нашел файл." << std::endl;

        code << file.rdbuf();

        auto tokens = Lexer(code.str()).tokenize();
        auto expression = Parser(tokens).parse();

        Environment env;

        for (const auto& expr : expression)
            expr->execute(env);
    }
}
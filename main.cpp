#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <filesystem>

#include "Parser/Lexer.h"
#include "Parser/Parser.h"

#include "Parser/AST/Statement.h"

#include "libs/Environment.h"
#include "libs/Console.h"

#if _WIN32
#include <windows.h>
#endif

#define VERSION "0.1"

void error_print(const std::string& name, const std::string& message,
                 int line = 0, const std::string& source = "", int column = 0)
{
    std::ostream& os = std::cerr;

    const std::string bar = Console::paint("│", Color::Gray, false, true);

    os << "\nОй!\n"
       << Console::paint("╭─ ", Color::Gray, false, true)
       << Console::paint("KidLang", Color::BrightCyan, true, true)
       << Console::paint(" · ", Color::Gray, false, true)
       << Console::paint(name, Color::BrightRed, true, true)
       << ' '
       << Console::paint(std::string("──────────────").c_str(), Color::Gray, false, true)
       << '\n';

    os << bar << '\n';

    os << bar << "  " << Console::paint(message, Color::BrightYellow, true, true) << '\n';

    if (line > 0 && !source.empty())
    {
        std::ostringstream num;
        num << std::setw(4) << line;

        os << bar << '\n'
           << bar << "  " << Console::paint(num.str(), Color::Blue, true, true)
           << ' ' << Console::paint("│", Color::Gray, false, true)
           << ' ' << source << '\n';

        if (column > 0)
        {
            os << bar << "  " << std::string(4, ' ')
               << ' ' << Console::paint("│", Color::Gray, false, true)
               << ' ' << std::string(column - 1, ' ')
               << Console::paint("^", Color::BrightRed, true, true) << '\n';
        }
    }
    else if (line > 0)
    {
        os << bar << '\n'
           << bar << "  " << Console::paint("строка " + std::to_string(line), Color::Blue, false, true) << '\n';
    }

    os << bar << '\n'
       << Console::paint("╰──────────────────────────────", Color::Gray, false, true)
       << '\n' << std::endl;
}

static std::string get_line(const std::string& code, int n)
{
    std::istringstream in(code);
    std::string s;
    for (int i = 1; std::getline(in, s); i++)
        if (i == n) return s;
    return "";
}

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

        try
        {
            auto tokens = Lexer(code.str()).tokenize();
            std::vector<std::unique_ptr<Statement>> program = Parser(tokens).parse();

            Environment env;
            for (const auto& st : program)
                st->execute(env);
        }
        catch (const ControlFlowSignal& e)
        {
            error_print("Ошибка в коде", e.what(), 1, get_line(code.str(), code.str().length()));
            return 1;
        }
        catch (const std::exception& e)
        {
            error_print("Ошибка", e.what());
            return 1;
        }
    }
}
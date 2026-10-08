//
// Created by Xomagat on 08.10.2026.
//

#ifndef KID_METHODS_H
#define KID_METHODS_H

#pragma once
#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "Value.h"
#include "NumberValue.h"
#include "StringValue.h"
#include "BoolValue.h"
#include "NothingValue.h"

using MethodArgs = std::vector<std::unique_ptr<Value>>;
using Method = std::function<std::unique_ptr<Value>(Value& self, MethodArgs& args)>;

inline std::string type_of(Value* v)
{
    if (dynamic_cast<NumberValue*>(v))  return "число";
    if (dynamic_cast<StringValue*>(v))  return "текст";
    if (dynamic_cast<BoolValue*>(v))    return "условие";
    return "ничего";
}

class Methods
{
    using Table = std::unordered_map<std::string, std::unordered_map<std::string, Method>>;
    using Constants = std::unordered_map<std::string, double>;

    // ---------- вспомогательные ----------
    static void need(const MethodArgs& a, size_t n, const std::string& name)
    {
        if (a.size() != n)
            throw std::runtime_error("Метод '" + name + "' ждёт аргументов: " + std::to_string(n));
    }

    static std::unique_ptr<Value> num(double x) { return std::make_unique<NumberValue>(x); }
    static std::unique_ptr<Value> str(const std::string& x) { return std::make_unique<StringValue>(x); }
    static std::unique_ptr<Value> boolean(bool x) { return std::make_unique<BoolValue>(x); }

    // Разбить UTF-8 строку на символы (каждый элемент — один символ)
    static std::vector<std::string> chars(const std::string& s)
    {
        std::vector<std::string> out;
        for (size_t i = 0; i < s.size();)
        {
            unsigned char c = s[i];
            size_t len = c < 0x80 ? 1 : (c >> 5) == 6 ? 2 : (c >> 4) == 14 ? 3 : (c >> 3) == 30 ? 4 : 1;
            out.push_back(s.substr(i, len));
            i += len;
        }
        return out;
    }

    static std::string join(const std::vector<std::string>& v, size_t from, size_t to)
    {
        std::string r;
        for (size_t i = from; i < to; i++) r += v[i];
        return r;
    }

    // Смена регистра для ASCII и кириллицы (двухбайтовый UTF-8)
    static std::string change_case(const std::string& s, bool upper)
    {
        std::string out;
        for (const auto& ch : chars(s))
        {
            if (ch.size() == 1)
            {
                out += (char)(upper ? std::toupper((unsigned char)ch[0])
                                    : std::tolower((unsigned char)ch[0]));
            }
            else if (ch.size() == 2)
            {
                unsigned cp = ((ch[0] & 0x1F) << 6) | (ch[1] & 0x3F);
                if (upper)
                {
                    if (cp >= 0x430 && cp <= 0x44F) cp -= 0x20;
                    else if (cp >= 0x450 && cp <= 0x45F) cp -= 0x50;
                }
                else
                {
                    if (cp >= 0x410 && cp <= 0x42F) cp += 0x20;
                    else if (cp >= 0x400 && cp <= 0x40F) cp += 0x50;
                }
                out += (char)(0xC0 | (cp >> 6));
                out += (char)(0x80 | (cp & 0x3F));
            }
            else out += ch;
        }
        return out;
    }

    static long long to_index(Value& v, const std::string& name)
    {
        double d = v.as_number();
        if (d != std::floor(d))
            throw std::runtime_error("Метод '" + name + "' ждёт целое число!");
        return (long long)d;
    }

    static Table init()
    {
        Table t;

        // ---------- любой тип ----------
        t["любой"]["в_текст"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 0, "в_текст");
            return str(s.as_string());
        };

        // ---------- текст ----------
        t["текст"]["длина"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 0, "длина");
            double n = 0;
            for (unsigned char c : s.as_string())
                if ((c & 0xC0) != 0x80) n++;
            return std::make_unique<NumberValue>(n);
        };
        t["текст"]["содержит"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 1, "содержит");
            return std::make_unique<BoolValue>(
                s.as_string().find(a[0]->as_string()) != std::string::npos);
        };
        t["текст"]["заменить"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 2, "заменить");
            std::string str = s.as_string(), from = a[0]->as_string(), to = a[1]->as_string();
            if (from.empty()) return std::make_unique<StringValue>(str);
            size_t p = 0;
            while ((p = str.find(from, p)) != std::string::npos)
            {
                str.replace(p, from.size(), to);
                p += to.size();
            }
            return std::make_unique<StringValue>(str);
        };
        t["текст"]["пусто"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 0, "пусто");
            return boolean(s.as_string().empty());
        };
        t["текст"]["в_верхний"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 0, "в_верхний");
            return str(change_case(s.as_string(), true));
        };
        t["текст"]["в_нижний"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 0, "в_нижний");
            return str(change_case(s.as_string(), false));
        };
        t["текст"]["обрезать"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 0, "обрезать");
            const std::string ws = " \t\r\n";
            std::string x = s.as_string();
            size_t b = x.find_first_not_of(ws);
            if (b == std::string::npos) return str("");
            size_t e = x.find_last_not_of(ws);
            return str(x.substr(b, e - b + 1));
        };
        t["текст"]["начинается_с"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 1, "начинается_с");
            const std::string x = s.as_string(), p = a[0]->as_string();
            return boolean(x.size() >= p.size() && x.compare(0, p.size(), p) == 0);
        };
        t["текст"]["заканчивается_на"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 1, "заканчивается_на");
            const std::string x = s.as_string(), p = a[0]->as_string();
            return boolean(x.size() >= p.size() && x.compare(x.size() - p.size(), p.size(), p) == 0);
        };
        t["текст"]["повторить"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 1, "повторить");
            long long n = to_index(*a[0], "повторить");
            if (n < 0) throw std::runtime_error("Нельзя повторить текст отрицательное число раз!");
            std::string x = s.as_string(), r;
            for (long long i = 0; i < n; i++) r += x;
            return str(r);
        };
        t["текст"]["перевернуть"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 0, "перевернуть");
            auto c = chars(s.as_string());
            std::reverse(c.begin(), c.end());
            return str(join(c, 0, c.size()));
        };
        t["текст"]["символ"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 1, "символ");
            auto c = chars(s.as_string());
            long long i = to_index(*a[0], "символ");
            if (i < 0 || i >= (long long)c.size())
                throw std::runtime_error("Индекс символа вне границ текста!");
            return str(c[i]);
        };
        // подтекст(начало, длина) — индексация с нуля
        t["текст"]["подтекст"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 2, "подтекст");
            auto c = chars(s.as_string());
            long long from = to_index(*a[0], "подтекст");
            long long len  = to_index(*a[1], "подтекст");
            if (from < 0 || len < 0 || from > (long long)c.size())
                throw std::runtime_error("Неверные границы для 'подтекст'!");
            size_t to = std::min<size_t>(c.size(), from + len);
            return str(join(c, from, to));
        };
        // найти(часть) — индекс символа или -1
        t["текст"]["найти"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 1, "найти");
            const std::string x = s.as_string();
            size_t p = x.find(a[0]->as_string());
            if (p == std::string::npos) return num(-1);
            double n = 0;
            for (size_t i = 0; i < p; i++)
                if (((unsigned char)x[i] & 0xC0) != 0x80) n++;
            return num(n);
        };
        t["текст"]["в_число"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 0, "в_число");
            const std::string x = s.as_string();
            try
            {
                size_t pos = 0;
                double d = std::stod(x, &pos);
                if (pos != x.size()) throw std::invalid_argument("tail");
                return num(d);
            }
            catch (const std::exception&)
            {
                throw std::runtime_error("Текст «" + x + "» нельзя превратить в число!");
            }
        };

        // ---------- число ----------
        t["число"]["модуль"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 0, "модуль");
            return num(std::fabs(s.as_number()));
        };
        t["число"]["округлить"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 0, "округлить");
            return num(std::round(s.as_number()));
        };
        t["число"]["корень"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 0, "корень");
            double x = s.as_number();
            if (x < 0) throw std::runtime_error("Корень из отрицательного числа!");
            return num(std::sqrt(x));
        };
        t["число"]["степень"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 1, "степень");
            return num(std::pow(s.as_number(), a[0]->as_number()));
        };
        t["число"]["знак"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 0, "знак");
            double x = s.as_number();
            return num(x > 0 ? 1 : x < 0 ? -1 : 0);
        };
        t["число"]["целое"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 0, "целое");
            double x = s.as_number();
            return boolean(x == std::floor(x));
        };
        t["число"]["чётное"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 0, "чётное");
            double x = s.as_number();
            return boolean(x == std::floor(x) && std::fmod(x, 2) == 0);
        };
        t["число"]["нечётное"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 0, "нечётное");
            double x = s.as_number();
            return boolean(x == std::floor(x) && std::fmod(x, 2) != 0);
        };
        t["число"]["минимум"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 1, "минимум");
            return num(std::min(s.as_number(), a[0]->as_number()));
        };
        t["число"]["максимум"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 1, "максимум");
            return num(std::max(s.as_number(), a[0]->as_number()));
        };
        t["число"]["ограничить"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 2, "ограничить");
            double lo = a[0]->as_number(), hi = a[1]->as_number();
            if (lo > hi) throw std::runtime_error("В 'ограничить' минимум больше максимума!");
            return num(std::clamp(s.as_number(), lo, hi));
        };
        t["число"]["синус"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 0, "синус");
            return num(std::sin(s.as_number()));
        };
        t["число"]["косинус"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 0, "косинус");
            return num(std::cos(s.as_number()));
        };
        t["число"]["логарифм"] = [](Value& s, MethodArgs& a) -> std::unique_ptr<Value> {
            need(a, 0, "логарифм");
            double x = s.as_number();
            if (x <= 0) throw std::runtime_error("Логарифм определён только для положительных чисел!");
            return num(std::log(x));
        };

        return t;
    }

    static Constants init_constants()
    {
        return {
            {"пи",             3.14159265358979323846},
            {"е",              2.71828182845904523536},
            {"бесконечность",  std::numeric_limits<double>::infinity()},
            {"максимум_числа", std::numeric_limits<double>::max()},
            {"минимум_числа",  std::numeric_limits<double>::lowest()},
        };
    }

public:
    // Метод конкретного типа; если нет — ищем среди универсальных ("любой")
    static const Method& get(const std::string& type, const std::string& name)
    {
        static const Table table = init();

        for (const char* key : { type.c_str(), "любой" })
        {
            auto t = table.find(key);
            if (t != table.end())
            {
                auto m = t->second.find(name);
                if (m != t->second.end()) return m->second;
            }
        }
        throw std::runtime_error("У типа «" + type + "» нет метода «" + name + "»!");
    }

    // ---------- встроенные переменные (константы) ----------
    static bool has_constant(const std::string& name)
    {
        static const Constants c = init_constants();
        return c.count(name) > 0;
    }

    static std::unique_ptr<Value> constant(const std::string& name)
    {
        static const Constants c = init_constants();
        auto it = c.find(name);
        if (it == c.end())
            throw std::runtime_error("Нет встроенной переменной «" + name + "»!");
        return std::make_unique<NumberValue>(it->second);
    }
};

#endif // KID_METHODS_H
//
// Created by Xomagat on 18.10.2026.
//

#ifndef KID_NATIVELIBS_H
#define KID_NATIVELIBS_H

#pragma once
#include <cmath>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "../Parser/AST/AssignmentStatement.h"
#include "Functions.h"
#include "ListValue.h"

using NativeArgs = std::vector<std::unique_ptr<Value>>;
using NativeFn   = std::function<std::unique_ptr<Value>(NativeArgs&)>;

class NativeFunction : public Function
{
    std::string name;
    size_t arity;
    NativeFn fn;

public:
    NativeFunction(std::string name, size_t arity, NativeFn fn)
        : name(std::move(name)), arity(arity), fn(std::move(fn)) {}

    std::unique_ptr<Value> execute(Environment&, NativeArgs args) const override
    {
        if (args.size() != arity)
            throw std::runtime_error("Рецепту «" + name + "» нужно аргументов: " + std::to_string(arity)
                + ", а ты дал: " + std::to_string(args.size()) + "!");
        return fn(args);
    }
};

class NativeLibs
{
    struct Entry { std::string name; size_t arity; NativeFn fn; };
    struct Const { std::string name, type; std::shared_ptr<Value> value; };
    struct Lib   { std::vector<Entry> funcs; std::vector<Const> consts; };

    static std::map<std::string, Lib>& table()
    {
        static std::map<std::string, Lib> t;
        return t;
    }

public:
    static bool add(const std::string& lib, const std::string& name, size_t arity, NativeFn fn)
    {
        table()[lib].funcs.push_back({name, arity, std::move(fn)});
        return true;
    }

    static bool add_const(const std::string& lib, const std::string& name,
                          const std::string& type, std::shared_ptr<Value> value)
    {
        table()[lib].consts.push_back({name, type, std::move(value)});
        return true;
    }

    static void load(const std::string& lib, Environment& env)
    {
        auto it = table().find(lib);
        if (it == table().end())
        {
            std::string known;
            for (auto& [name, l] : table()) known += " «" + name + "»";
            throw std::runtime_error("Я не знаю библиотеку «" + lib + "». Я знаю:"
                + (known.empty() ? " ничего" : known));
        }

        for (auto& e : it->second.funcs)
            Functions::define(e.name, std::make_unique<NativeFunction>(e.name, e.arity, e.fn));

        for (auto& c : it->second.consts)
            env.root().define(c.type, c.name, c.value->clone());
    }
};

namespace native
{
    template<class T> inline constexpr bool always_false = false;

    template<class U>
    U from_value(Value& v, const std::string& fn, size_t i)
    {
        auto bad = [&](const char* want) {
            return std::runtime_error("Рецепт «" + fn + "» ждёт «" + want + "» в аргументе номер "
                + std::to_string(i + 1) + ", а ты дал «" + type_name(&v) + "»!");
        };

        if constexpr (std::is_same_v<U, bool>)
        {
            if (!dynamic_cast<BoolValue*>(&v)) throw bad("условие");
            return v.as_bool();
        }
        else if constexpr (std::is_same_v<U, std::string>)
        {
            if (!dynamic_cast<StringValue*>(&v)) throw bad("текст");
            return v.as_string();
        }
        else if constexpr (std::is_arithmetic_v<U>)
        {
            if (!dynamic_cast<NumberValue*>(&v)) throw bad("число");
            double d = v.as_number();
            if constexpr (std::is_integral_v<U>)
                if (d != std::floor(d))
                    throw std::runtime_error("Рецепту «" + fn + "» нужно целое число в аргументе номер "
                        + std::to_string(i + 1) + "!");
            return static_cast<U>(d);
        }
        else static_assert(always_false<U>, "Неподдерживаемый тип аргумента в библиотеке");
    }

    template<class R>
    std::unique_ptr<Value> to_value(R&& r)
    {
        using U = std::decay_t<R>;
        if constexpr (std::is_same_v<U, bool>)       return std::make_unique<BoolValue>(r);
        else if constexpr (std::is_arithmetic_v<U>)  return std::make_unique<NumberValue>(static_cast<double>(r));
        else if constexpr (std::is_convertible_v<U, std::string>)
                                                     return std::make_unique<StringValue>(std::string(r));
        else static_assert(always_false<U>, "Неподдерживаемый тип результата в библиотеке");
    }

    template<class R, class... A, size_t... I>
    std::unique_ptr<Value> invoke(R (*f)(A...), const std::string& name, NativeArgs& a,
                                  std::index_sequence<I...>)
    {
        (void)a; (void)name;
        if constexpr (std::is_void_v<R>)
        {
            f(from_value<std::decay_t<A>>(*a[I], name, I)...);
            return std::make_unique<NothingValue>();
        }
        else
            return to_value(f(from_value<std::decay_t<A>>(*a[I], name, I)...));
    }

    template<class R, class... A>
    bool reg(const char* lib, const char* name, R (*f)(A...))
    {
        std::string n = name;
        return NativeLibs::add(lib, n, sizeof...(A), [f, n](NativeArgs& a) {
            return invoke(f, n, a, std::index_sequence_for<A...>{});
        });
    }

    template<class T>
    bool reg_const(const char* lib, const char* name, const T& v)
    {
        using U = std::decay_t<T>;
        const char* type = std::is_same_v<U, bool>  ? "условие"
                         : std::is_arithmetic_v<U>  ? "число"
                                                    : "текст";
        return NativeLibs::add_const(lib, name, type, std::shared_ptr<Value>(to_value(v)));
    }
}

#define KL_CAT2(a, b) a##b
#define KL_CAT(a, b)  KL_CAT2(a, b)

#define KL_EXPORT(fn) \
    [[maybe_unused]] static const bool KL_CAT(kl_reg_, __LINE__) = native::reg(KL_LIB, #fn, &fn);

#define KL_CONST(name) \
    [[maybe_unused]] static const bool KL_CAT(kl_const_, __LINE__) = native::reg_const(KL_LIB, #name, name);

#endif // KID_NATIVELIBS_H
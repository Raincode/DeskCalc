#include "Parser.hpp"

#include <cmath>
#include <iostream>

#include <mps/stl_util.hpp>
#include <mps/str_util.hpp>

#include "Warning.hpp"

// https://stackoverflow.com/questions/12500411/setting-a-stdfunction-variable-to-refer-to-the-stdsin-function
#define MAKE_FUNC(f) [] (const Complex& c) { return (f)(c); }
#define MAKE_REAL_FUNC(f) [] (double d) { return (f)(d); }

Parser::Parser(Lexer::TokenStream& ts)
    : ts{ ts }
{
    table.set_const("i", { 0, 1 });
    table.set_const("pi", std::acos(-1));
    table.set_const("e", 2.7182818284590452354);

    table.add_builtin_func("sin", MAKE_FUNC(std::sin));
    table.add_builtin_func("cos", MAKE_FUNC(std::cos));
    table.add_builtin_func("tan", MAKE_FUNC(std::tan));
    table.add_builtin_func("asin", MAKE_FUNC(std::asin));
    table.add_builtin_func("acos", MAKE_FUNC(std::acos));
    table.add_builtin_func("atan", MAKE_FUNC(std::atan));
    table.add_builtin_func("sinh", MAKE_FUNC(std::sinh));
    table.add_builtin_func("cosh", MAKE_FUNC(std::cosh));
    table.add_builtin_func("tanh", MAKE_FUNC(std::tanh));

    table.add_builtin_func("abs", MAKE_FUNC(std::abs));
    table.add_builtin_func("norm", MAKE_FUNC(std::norm));
    table.add_builtin_func("arg", MAKE_FUNC(std::arg));
    table.add_builtin_func("exp", MAKE_FUNC(std::exp));

    table.add_builtin_func("sqrt", MAKE_FUNC(std::sqrt));
    table.add_builtin_func("ln", MAKE_FUNC(std::log));
    table.add_builtin_func("log", MAKE_FUNC(std::log10));

    table.add_builtin_real_func("floor", MAKE_REAL_FUNC(std::floor));
    table.add_builtin_real_func("ceil", MAKE_REAL_FUNC(std::ceil));
    table.add_builtin_real_func("round", MAKE_REAL_FUNC(std::round));
    table.add_builtin_real_func("trunc", MAKE_REAL_FUNC(std::trunc));

    error.set_cleanup_func([this] { lparenCount = 0; });
}

void Parser::parse()
{
    hasResult = true;
    ts.get();
    res = expr();
}

void Parser::parse(const std::string& str)
{
    ts.set_input(str);
    parse();
}

Complex Parser::expr()
{
    auto left = term();
    for (;;) {
        if (consume(Kind::Plus)) {
            left += term();
        }
        else if (consume(Kind::Minus)) {
            left -= term();
        }
        else if (peek(Kind::RParen)) {
            consume_closing_paren();
            if (consume(Kind::String)) {
                return left * table.value_of(prevToken.str);
            }
            return left;
        }
        else if (consume(Kind::Print) || consume(Kind::End)) {
            check_all_paren_closed();
            return left;
        }
        else {
            error("Syntax error");
        }
    }
}

Complex Parser::term()
{
    auto left = sign();
    for (;;) {
        if (consume(Kind::Mul)) {
            left *= sign();
        }
        else if (consume(Kind::Div)) {
            left = safe_div(left, sign());
        }
        else if (consume(Kind::FloorDiv)) {
            left = safe_floordiv(left, sign());
        }
        else if (consume(Kind::Mod)) {
            auto right = sign();
            if (!left.imag() && !right.imag()) {
                auto left_trunc = std::trunc(left.real());
                auto right_trunc = std::trunc(right.real());
                if (left.real() != left_trunc || right.real() != right_trunc) {
                    error("Modulo not defined for floating point numbers");
                }
                left = safe_mod(static_cast<long>(left_trunc), static_cast<long>(right_trunc));
            }
            else {
                error("Modulo not defined for complex numbers (yet)");
            }
        }
        else if (consume(Kind::Parallel)) {
            auto right = sign();
            if ((left.imag() && right.imag()) ||
                (left.real() > 0 && right.real() > 0)) {
                left = left * right / (left + right);
            }
            else {
                error("Resistors must be greater than 0");
            }
        }
        else {
            return left;
        }
    }
}

Complex Parser::sign()
{
    if (consume(Kind::Plus)) {
        return postfix();
    }
    else if (consume(Kind::Minus)) {
        return -postfix();
    }
    else {
        return postfix();
    }
}

Complex Parser::postfix()
{
    auto left = prim();
    for (;;) {
        if (consume(Kind::Pow)) {
            return std::pow(left, sign());
        }
        else if (consume(Kind::Fac)) {
            static Warning w{ "Note: Factorial might overflow for larger numbers" };
            double trunc = std::trunc(left.real());
            if (!left.imag() && trunc == left.real() && trunc >= 0) {
                return factorial(static_cast<int>(trunc));
            }
            error("Factorial only defined for N and 0");
        }
        else if (consume(Kind::String)) {
            return left * table.value_of(prevToken.str);
        }
        else {
            return left;
        }
    }
}

Complex Parser::prim()
{
    if (consume(Kind::Number)) {
        return prevToken.num;
    }
    else if (consume(Kind::String)) {
        return resolve_string_token();
    }
    else if (consume(Kind::LParen)) {
        auto val = expr();
        return val;
    }
    error("Unexpected Token ", ts.current());
    return 0; // silence warning
}

Complex Parser::resolve_string_token()
{
    auto str = prevToken.str;
    if (table.has_func(str)) {
        expect(Kind::LParen);
        return table.call_func(str, expr());
    }
    else if (consume(Kind::Assign)) {
        table.set_var(str, expr());
        return table.value_of(str);
    }
    else if (table.has_var(str)) {
        return table.value_of(str);
    }
    else if (consume(Kind::LParen)) {
        error("Defining custom functions not implemented yet");
    }
    else {
        error(str, " is undefined");
    }
    return 0; // silence warning
}

void Parser::check_open_paren()
{
    if (!lparenCount) {
        lparenCount = 0;
        error("Unexpected Token: )");
    }
}

void Parser::check_all_paren_closed()
{
    if (lparenCount) {
        lparenCount = 0;
        error("Expected Token: )");
    }
}

void Parser::consume_closing_paren()
{
    while (consume(Kind::RParen)) {
        check_open_paren();
        --lparenCount;
    }
}

bool Parser::consume(Kind kind)
{
    if (ts.current().kind == kind) {
        prevToken = ts.current();
        if (kind == Kind::LParen) {
            ++lparenCount;
        }
        ts.get();
        return true;
    }
    return false;
}

bool Parser::peek(Kind kind) const
{
    return ts.current().kind == kind;
}

void Parser::expect(Kind kind)
{
    if (!consume(kind)) {
        error("Expected Token ", Token{ kind });
    }
}

Complex safe_div(Complex left, Complex right)
{
    if (right.real() || right.imag()) {
        return left / right;
    }
    throw std::runtime_error{ "Divide by zero" };
}

Complex safe_floordiv(Complex left, Complex right)
{
    if (right.real() || right.imag()) {
        left /= right;
        return { std::floor(left.real()), std::floor(left.imag()) };
    }
    throw std::runtime_error{ "Divide by zero" };
}

unsigned factorial(int n)
{
    if (n < 0) {
        throw std::runtime_error{ "Factorial not defined for negative numbers" };
    }
    else if (n == 1) {
        return 0;
    }
    else {
        unsigned result{ 1 };
        for (int i = 2; i <= n; ++i) {
            result *= i;
        }
        return result;
    }
}
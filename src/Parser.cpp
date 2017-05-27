#include "Parser.hpp"

#include <cmath>
#include <iostream>

#include <mps/stl_util.hpp>
#include <mps/str_util.hpp>

#include "Function.hpp"

using std::runtime_error;
using std::logic_error;

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
        if (consume(Kind::Plus)) 
            left += term();
        else if (consume(Kind::Minus)) 
            left -= term();
        else if (peek(Kind::RParen)) {
            consume_closing_paren();
            return left;
        }
        else if (consume(Kind::Print) || consume(Kind::End)) {
            check_all_paren_closed();
            return left;
        }
        else if (peek(Kind::Comma)) {
            return left;
        }
        else error("Syntax error");
    }
}

Complex calc_Rparallel(Complex R1, Complex R2)
{
    if ((R1.imag() && R2.imag()) || (R1.real() > 0 && R2.real() > 0))
        return R1 * R2 / (R1 + R2);
    throw runtime_error{ "Resistors must be greater than 0 or complex" };
}

Complex Parser::term()
{
    auto left = sign();
    for (;;) {
        if (consume(Kind::Mul))
            left *= sign();
        else if (consume(Kind::Div))
            left = safe_div(left, sign());
        else if (consume(Kind::FloorDiv))
            left = safe_floordiv(left, sign());
        else if (consume(Kind::Mod)) {
            auto right = sign();
            if (!left.imag() && !right.imag()) {
                auto left_trunc = std::trunc(left.real());
                auto right_trunc = std::trunc(right.real());
                if (left.real() != left_trunc || right.real() != right_trunc)
                    error("Modulo not defined for floating point numbers");
                left = safe_mod(static_cast<long>(left_trunc), static_cast<long>(right_trunc));
            }
            else error("Modulo not defined for complex numbers (yet)");
        }
        else if (consume(Kind::Parallel))
            left = calc_Rparallel(left, sign());
        else return left;
    }
}

Complex Parser::sign()
{
    if (consume(Kind::Plus)) return postfix();
    if (consume(Kind::Minus)) return -postfix();
    return postfix();
}

Complex Parser::postfix()
{
    auto left = prim();
    for (;;) {
        if (consume(Kind::Pow)) return std::pow(left, sign());
        if (consume(Kind::String)) return left * table.value_of(prevToken.str);
        if (consume(Kind::Fac)) {
            double trunc{ std::trunc(left.real()) };
            if (!left.imag() && trunc == left.real() && trunc >= 0)
                return factorial(static_cast<int>(trunc));
            error("Factorial only defined for N and 0");
        }
        return left;
    }
}

Complex Parser::prim()
{
    if (consume(Kind::Number)) return prevToken.num;
    if (consume(Kind::String)) return resolve_string_token();
    if (consume(Kind::LParen)) return expr();
    error("Unexpected Token ", ts.current());
    throw std::logic_error{ "Fall through prim()" }; // silence C4715
}

Complex Parser::resolve_string_token()
{
    auto str = prevToken.str;
    if (table.has_func(str)) {
        expect(Kind::LParen);
        return table.call_func(str, expr());
    }
    if (table.has_user_func(str)) {
        expect(Kind::LParen);
        Args args;
        do { 
            args.push_back(expr());
        } while (consume(Kind::Comma));
        return table.call_user_func(str, args);
    }
    if (consume(Kind::Assign)) {
        table.set_var(str, expr());
        return table.value_of(str);
    }
    if (table.has_var(str))
        return table.value_of(str);
    if (consume(Kind::LParen)) {
        Function f;
        f.set_name(str);
        do {
            expect(Kind::String);
            f.add_var(prevToken.str);
        } while (consume(Kind::Comma));

        consume_closing_paren();
        expect(Kind::Assign);

        std::ostringstream term;
        while (!peek(Kind::Print) && !peek(Kind::End) && !peek(Kind::Invalid)) {
            term << ts.current();
            ts.get();
        }
        f.set_term(term.str());
        table.set_custom_func(str, f);
        hasResult = false;
        return 0;
    }
    error(str, " is undefined");
    throw logic_error{ "Fall through resolve_string_token()" }; // silence C4715
}

void Parser::check_open_paren()
{
    if (lparenCount) return;
    lparenCount = 0;
    error("Unexpected Token: )");
}

void Parser::check_all_paren_closed()
{
    if (!lparenCount) return;
    lparenCount = 0;
    error("Expected Token: )");
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
        if (kind == Kind::LParen) ++lparenCount;
        prevToken = ts.current();
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
    if (!consume(kind)) error("Expected Token ", kind);
}

Complex safe_div(Complex left, Complex right)
{
    if (right.real() || right.imag())
        return left / right;
    throw runtime_error{ "Divide by zero" };
}

Complex safe_floordiv(Complex left, Complex right)
{
    if (!right.real() && !right.imag()) 
        throw runtime_error{ "Divide by zero" };
    left /= right;
    return { std::floor(left.real()), std::floor(left.imag()) };
}

unsigned factorial(int n)
{
    if (n < 0) throw runtime_error{ "Factorial not defined for negative numbers" };
    unsigned ret{ 1 };
    while (n--) ret *= n;
    return ret;
}
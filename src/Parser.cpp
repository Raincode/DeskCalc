#include "Parser.hpp"

#include <cmath>
#include <iostream>

#include <mps/stl_util.hpp>
#include <mps/str_util.hpp>

#include "Function.hpp"
#include "math_util.hpp"

Parser::Parser(SymbolTable& table)
    : table{ table }
{
}

void Parser::parse()
{
    hasResult = true;
    ts.get();
    res = expr();
    expect(Kind::End);
}

void Parser::parse(std::istream& is)
{
    ts.set_input(is);
    parse();
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
        else
            return left;
    }
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
        else if (consume(Kind::Mod))
            left = safe_mod(left, sign());
        else if (consume(Kind::Parallel))
            left = calc_Rparallel(left, sign());
        else 
            return left;
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
        if (consume(Kind::String)) return left * table.value_of(prevTok.str);
        if (consume(Kind::Fac)) return checked_factorial(left);
        return left;
    }
}

Complex Parser::prim()
{
    if (consume(Kind::Number)) return prevTok.num;
    if (consume(Kind::String)) return resolve_string_token();
    if (consume(Kind::LParen)) {
        auto val = expr();
        expect(Kind::RParen);
        return val;
    }
    error("Unexpected Token ", ts.current());
    throw std::logic_error{ "Fall through prim()" }; // silence C4715
}

Complex Parser::resolve_string_token()
{
    auto str = prevTok.str;
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
        expect(Kind::RParen);
        return table.call_user_func(str, args);
    }
    if (consume(Kind::Assign)) {
        table.set_var(str, expr());
        return table.value_of(str);
    }
    if (table.has_var(str))
        return table.value_of(str);
    if (consume(Kind::LParen)) {
        Function f{ table };
        f.set_name(str);
        do {
            expect(Kind::String);
            f.add_var(prevTok.str);
        } while (consume(Kind::Comma));

        expect(Kind::RParen);
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
    throw std::logic_error{ "Fall through resolve_string_token()" }; // silence C4715
}

bool Parser::consume(Kind kind)
{
    if (ts.current().kind == kind) {
        prevTok = ts.current();
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
#include "Parser.hpp"

#include <cmath>
#include <iostream>

#include <mps/stl_util.hpp>
#include <mps/str_util.hpp>

#include "Function.hpp"
#include "math_util.hpp"
#include "SymbolTable.hpp"

Parser::Parser(SymbolTable& table)
    : table{ table } { }

void Parser::parse()
{
    ts.get();
    while (!consume(Kind::End))
        stmt();
}

void Parser::stmt()
{
    hasResult = true;
    res = expr();
    consume(Kind::Print);
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
            left = impedance_parallel(left, sign());
        else 
            return left;
    }
}

Complex Parser::sign()
{
    if (consume(Kind::Plus))
        return postfix();
    if (consume(Kind::Minus)) 
        return -postfix();
    return postfix();
}

Complex Parser::postfix()
{
    auto left = prim();
    for (;;) {
        if (consume(Kind::Pow))
            return std::pow(left, sign());
        if (consume(Kind::String)) 
            return left * table.value(prevTok.str);
        if (consume(Kind::Fac)) 
            return factorial(left);
        return left;
    }
}

Complex Parser::prim()
{
    if (consume(Kind::Number)) 
        return prevTok.num;
    if (consume(Kind::String)) 
        return resolve_string_token();
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
    if (peek(Kind::LParen))
        return resolve_func();
    return resolve_var();
}

Complex Parser::resolve_var()
{
    auto name = prevTok.str;
    if (consume(Kind::Assign))
        table.set_var(name, expr());
    return table.value(name);
}

Args Parser::parse_args()
{
    expect(Kind::LParen);
    Args args;
    do {
        args.push_back(expr());
    } while (consume(Kind::Comma));
    expect(Kind::RParen);
    return args;
}

Complex Parser::func_call()
{
    auto funcName = prevTok.str;
    expect(Kind::LParen);
    auto result = table.value(funcName, expr());
    expect(Kind::RParen);
    return result;
}

void Parser::parse_param_list(Function& func)
{
    expect(Kind::LParen);
    do {
        expect(Kind::String);
        func.add_var(prevTok.str);
    } while (consume(Kind::Comma));
    expect(Kind::RParen);
}

void Parser::parse_func_term(Function& func)
{
    std::ostringstream term;
    while (!peek(Kind::Print) && !peek(Kind::End) && !peek(Kind::Invalid)) {
        term << ts.current();
        ts.get();
    }
    func.set_term(term.str());
}

Complex Parser::resolve_func()
{
    auto name = prevTok.str;
    if (table.has_func(name))
        return func_call();
    if (table.has_user_func(name))
        return table.value(name, parse_args());

    Function f{ table };
    f.set_name(name);
    parse_param_list(f);

    expect(Kind::Assign);
    parse_func_term(f);

    Args test_args(f.num_params(), 1);
    f(test_args);
    table.set_custom_func(name, f);
    hasResult = false;
    return 0;
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
    if (!consume(kind))
        error("Expected Token ", kind);
}
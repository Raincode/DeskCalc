#include "Parser.hpp"

#include <cmath>
#include <iostream>

#include <mps/stl_util.hpp>
#include <mps/str_util.hpp>

#include "Function.hpp"
#include "math_util.hpp"
#include "SymbolTable.hpp"

Parser::Parser(SymbolTable& table)
    : table{ table } 
{ 
}

void Parser::parse()
{
    ts.get();
    while (!consume(Kind::End))
        stmt();
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

void Parser::stmt()
{
    hasResult = true;
    if (peek(Kind::FuncDef)) {
        func_def();
        hasResult = false;
    }
    else if (!peek(Kind::Print))
        res = expr();
    if (!peek(Kind::End))
        expect(Kind::Print);
}

void Parser::func_def()
{
    expect(Kind::FuncDef);
    expect(Kind::String);
    Function func{ prevTok.str, table };

    parse_param_list(func);
    expect(Kind::Assign);

    parse_func_term(func);
    List test_args(func.numArgs(), 1);
    func(test_args);

    table.set_func(func.name(), func);
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
    if (consume(Kind::Minus)) 
        return -postfix();
    consume(Kind::Plus);
    return postfix();
}

Complex Parser::postfix()
{
    auto left = prim();
    for (;;) {
        if (consume(Kind::Pow))
            return pretty_pow(left, sign());
        else if (consume(Kind::String))
            return left * table.value_of(prevTok.str);
        else if (consume(Kind::Fac))
            left = factorial(left);
        else
            return left;
    }
}

Complex Parser::prim()
{
    if (consume(Kind::Number)) 
        return prevTok.num;
    if (consume(Kind::String))
        return resolve_str_tok();
    if (consume(Kind::LParen)) {
        auto val = expr();
        expect(Kind::RParen);
        return val;
    }
    error("Unexpected Token ", ts.current());
    throw std::logic_error{ "Fall through prim()" }; // silence C4715
}

Complex Parser::resolve_str_tok()
{
    if (peek(Kind::LParen))
        return func_call();
    if (peek(Kind::Assign))
        return var_def();
    if (table.has_list(prevTok.str)) {
        if (!peek(Kind::Print) && !peek(Kind::End))
            error("Unexpected Token ", ts.current());
        print_list(std::cout, table.list(prevTok.str));
        std::cout << '\n';
        return no_result();
    }
    return table.value_of(prevTok.str);
}

List Parser::list_elems()
{
    List list;
    if (!peek(Kind::RParen) && !peek(Kind::RBracket)) {
        do {
            list.emplace_back(expr());
        } while (consume(Kind::Comma));
    }
    return list;
}

List Parser::list()
{
    expect(Kind::LBracket);
    auto l = list_elems();
    expect(Kind::RBracket);
    return l;
}

Complex Parser::no_result()
{
    hasResult = false;
    return 0; // dummy value
}

Complex Parser::var_def()
{
    auto name = prevTok.str;
    expect(Kind::Assign);
    if (peek(Kind::LBracket)) {
        table.set_list(name, list());
        return no_result();
    }
    if (table.isset(name) && !table.has_var(name))
        error(name, " is already defined");
    auto val = expr();
    table.set_var(name, val);
    return val;
}

Complex Parser::func_call()
{
    auto name = prevTok.str;
    return table.call_func(name, arg_list());
}

List Parser::arg_list()
{
    expect(Kind::LParen);
    List args;
    if (peek(Kind::String) && table.has_list(ts.current().str)) {
        consume(Kind::String);
        args = table.list(prevTok.str);
    }
    else {
        args = list_elems();
        if (args.empty())
            error("Invalid empty argument list");
    }
    expect(Kind::RParen);
    return args;
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
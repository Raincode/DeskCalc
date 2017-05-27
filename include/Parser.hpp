#pragma once

#include <complex>
#include <istream>
#include <map>
#include <string>

#include "ErrorReporter.hpp"
#include "Lexer.hpp"
#include "SymbolTable.hpp"

class Parser {
public:
    explicit Parser(Lexer::TokenStream& lexer);

    void parse();
    void parse(const std::string& input);
    Complex result() const { return res; }
    bool has_result() const { return hasResult; }
    SymbolTable& symbol_table() { return table; }

private:
    Complex expr();
    Complex term();
    Complex sign();
    Complex postfix();
    Complex prim();

    bool consume(Kind kind);
    bool peek(Kind kind) const;
    void expect(Kind kind);

    void check_all_paren_closed();
    void check_open_paren();
    void consume_closing_paren();

    Complex resolve_string_token();

    Token prevToken;
    SymbolTable table;
    Lexer::TokenStream& ts;
    int lparenCount{};
    Complex res;
    bool hasResult{};
    ErrorReporter error;
};

Complex safe_div(Complex left, Complex right);
Complex safe_floordiv(Complex left, Complex right);
template<class T>
T safe_mod(T left, T right)
{
    if (right) {
        return left % right;
    }
    throw std::runtime_error{ "Divide by Zero" };
}
unsigned factorial(int n);


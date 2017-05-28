#pragma once

#include <istream>
#include <map>
#include <string>

#include "ErrorReporter.hpp"
#include "Function.hpp"
#include "SymbolTable.hpp"
#include "TokenStream.hpp"
#include "types.hpp"

class Parser {
public:
    Parser(SymbolTable& table);

    void parse(std::istream& is);
    void parse(const std::string& input);

    Complex result() const { return res; }
    bool has_result() const { return hasResult; }

    SymbolTable& symbol_table() { return table; }
    void set_symbol_table(SymbolTable& t) { table = t; }

private:
    void parse();
    Complex expr();
    Complex term();
    Complex sign();
    Complex postfix();
    Complex prim();
    Complex resolve_string_token();
    Complex resolve_var();
    Complex resolve_func();
    Args parse_args();
    Complex func_call();
    void parse_param_list(Function& func);
    void parse_func_term(Function& func);

    bool peek(Kind kind) const;
    bool consume(Kind kind);
    void expect(Kind kind);

    Token prevTok;
    SymbolTable& table;
    TokenStream ts;
    ErrorReporter error;

    Complex res;
    bool hasResult{};
};




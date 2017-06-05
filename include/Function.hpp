#pragma once

#include <string>
#include <vector>

#include "types.hpp"

class SymbolTable;

class Function {
public:
    Function(const std::string& name, SymbolTable& table);

    Complex operator()(const List& args) const;

    void set_term(const std::string& t) { term = t; }
    void add_var(const std::string& v) { vars.push_back(v); }

    std::size_t numArgs() const { return vars.size(); }
    const std::string& name() const { return funcName; }

    friend std::ostream& operator<<(std::ostream& os, const Function& func);

private:
    SymbolTable& table;
    std::string funcName;
    std::string term;
    std::vector<std::string> vars;
};
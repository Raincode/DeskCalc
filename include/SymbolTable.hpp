#pragma once

#include <map>
#include <string>

#include "mps/stl_util.hpp"

#include "types.hpp"

class SymbolTable {
public:
    using ConstStrRef = const std::string&;

    SymbolTable();

    void add_constants();
    bool is_reserved_func(const std::string& name) const;

    void set_var(ConstStrRef name, Complex value);
    void set_list(ConstStrRef name, List&& list);
    void add_func(ConstStrRef name, Func func);

    Complex value_of(ConstStrRef var) const;
    const List& list(ConstStrRef var) const;
    Complex call_func(ConstStrRef func, const List& args) const;

    bool has_var(ConstStrRef name) const;
    bool has_list(ConstStrRef name) const;
    bool has_func(ConstStrRef name) const;
    bool isset(ConstStrRef name) const;

    void remove_var(ConstStrRef name);
    void remove_list(ConstStrRef name);
    void remove_func(ConstStrRef name);
    void remove_symbol(ConstStrRef name);

private:
    static const std::map<std::string, Func> defaultFuncTable;

    std::map<std::string, Complex> varTable;
    std::map<std::string, List> listTable;
    std::map<std::string, Func> funcTable;
};
#pragma once

#include <map>
#include <string>

#include "mps/stl_util.hpp"

#include "types.hpp"

class SymbolTable {
public:
    using ConstStrRef = const std::string&;

    struct Var {
        Complex val;
        bool isConst{};
    };

    SymbolTable() = default;

    void add_constants();
    void add_trig_funcs();
    void add_temp_conversions();
    void add_default_funcs();
    void add_list_funcs();

    void set_var(ConstStrRef name, Complex value);
    void add_const(ConstStrRef name, Complex value);
    void set_list(ConstStrRef name, List&& list);
    void set_func(ConstStrRef name, Func func);

    Complex value_of(ConstStrRef var) const;
    const List& list(ConstStrRef var) const;
    Complex call_func(ConstStrRef func, const List& args) const;

    bool is_const(ConstStrRef name) const;
    bool has_var(ConstStrRef name) const;
    bool has_list(ConstStrRef name) const;
    bool has_func(ConstStrRef name) const;
    bool isset(ConstStrRef name) const;

    void remove_var(ConstStrRef name);
    void remove_list(ConstStrRef name);
    void remove_func(ConstStrRef name);

private:
    std::map<std::string, Var> varTable;
    std::map<std::string, List> listTable;
    std::map<std::string, Func> funcTable;
};

SymbolTable::Var make_var(Complex val);
SymbolTable::Var make_const(Complex val);
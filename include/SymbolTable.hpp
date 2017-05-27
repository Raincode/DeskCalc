#pragma once

#include <complex>
#include <functional>
#include <map>
#include <string>

#include "mps/stl_util.hpp"

#include "types.hpp"

class SymbolTable {
public:
    struct Var {
        enum Type { Mutable, Const };
        Complex value;
        Type type{};
    };

    SymbolTable() = default;

    void add_default_funcs();
    void add_constants();

    Complex value_of(const std::string& var);
    void set_var(const std::string& name, Complex value);
    void set_const(const std::string& name, Complex value);
    bool has_var(const std::string& name) const;
    void erase_var(const std::string& name);

    Complex call_func(const std::string& name, Complex arg);
    Complex call_user_func(const std::string& name, const Args& args);
    void add_builtin_func(const std::string& name, ComplexFunc func);
    void add_builtin_real_func(const std::string& name, std::function<double(double)> func);
    void set_custom_func(const std::string& name, ComplexFunc func);
    void set_custom_func(const std::string& name, ComplexMultiFunc func);
    bool has_func(const std::string& name) const;
    bool has_user_func(const std::string& name) const;
    void erase_func(const std::string& name);

private:
    void set_value(const std::string& name, Var var);
    void set_function(const std::string& name, ComplexFunc func);

    std::map<std::string, Var> varTable;
    std::map<std::string, ComplexFunc> funcTable;
    std::map<std::string, ComplexMultiFunc> userFuncTable;
};

class VarGuard {
public:
    VarGuard(SymbolTable& table)
        : table{ table } { }
    
    ~VarGuard()
    {
        for (const auto& var : setVars)
            table.erase_var(var);
        for (const auto& pair : varCache)
            table.set_var(pair.first, pair.second);
    }

    void set_temp(const std::string& name, Complex val)
    {
        if (mps::stl::contains(setVars, name))
            throw std::runtime_error{ "VarGuard - Cannot shadow variable " + name + " twice." };

        if (table.has_var(name))
            varCache[name] = table.value_of(name);

        table.set_var(name, val);
        setVars.push_back(name);
    }

private:
    SymbolTable& table;
    std::map<std::string, Complex> varCache;
    std::vector<std::string> setVars;
};
#pragma once

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

    Complex value(const std::string& var);
    Complex value(const std::string& funcName, Complex arg);
    Complex value(const std::string& userFunc, const Args& args);

    void set_var(const std::string& name, Complex value);
    Var get_var(const std::string& name) const;
    void set_const(const std::string& name, Complex value);
    bool has_var(const std::string& name) const;
    void erase_var(const std::string& name);

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
        for (const auto& pair : varCache) {
            if (pair.second.type == SymbolTable::Var::Const)
                table.set_const(pair.first, pair.second.value);
            else
                table.set_var(pair.first, pair.second.value);
        }
    }

    void set_temp(const std::string& name, Complex val)
    {
        if (mps::stl::contains(setVars, name))
            throw std::runtime_error{ "VarGuard - Cannot shadow variable " + name + " twice." };

        if (table.has_var(name)) {
            varCache[name] = table.get_var(name);
            table.erase_var(name);
        }

        table.set_var(name, val);
        setVars.push_back(name);
    }

private:
    SymbolTable& table;
    std::map<std::string, SymbolTable::Var> varCache;
    std::vector<std::string> setVars;
};
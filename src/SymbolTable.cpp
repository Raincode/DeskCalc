#include "SymbolTable.hpp"

#include <cassert>
#include <stdexcept>

#include <mps/stl_util.hpp>

Complex SymbolTable::value_of(const std::string& var)
{
    auto found = varTable.find(var);
    if (found == end(varTable))
        throw std::runtime_error{ var + " is undefined." };
    return found->second.value;
}

void SymbolTable::set_var(const std::string& name, Complex value)
{
    set_value(name, { value, Var::Mutable });
}

void SymbolTable::set_const(const std::string& name, Complex value)
{
    set_value(name, { value, Var::Const });
}

bool SymbolTable::has_var(const std::string& name) const
{
    return mps::stl::contains(varTable, name);
}

void SymbolTable::erase_var(const std::string& name)
{
    auto found = varTable.find(name);
    assert(found != end(varTable));
    varTable.erase(found);
}

void SymbolTable::set_value(const std::string& name, Var var)
{
    auto found = varTable.find(name);
    if (found != end(varTable) && found->second.type == Var::Const)
        throw std::runtime_error{ "Cannot override constant " + name };
    varTable[name] = var;
}

Complex SymbolTable::call_func(const std::string& name, Complex arg)
{
    auto found = funcTable.find(name);
    if (found == end(funcTable))
        throw std::runtime_error{ name + " is undefined." };
    return found->second(arg);
}

Complex SymbolTable::call_user_func(const std::string & name, const Args & args)
{
    auto found = userFuncTable.find(name);
    if (found == end(userFuncTable))
        throw std::runtime_error{ name + " is undefined." };
    return found->second(args);
}

void SymbolTable::add_builtin_func(const std::string& name, ComplexFunc func)
{
    set_function(name, func);
}

void SymbolTable::add_builtin_real_func(const std::string& name, std::function<double(double)> func)
{
    auto proxy_func = [f = std::move(func), name](const Complex& c) {
        if (c.imag()) {
            throw std::runtime_error{ name + " not defined for complex numbers" };
        }
        return Complex{ f(c.real()), 0 };
    };
    set_function(name, proxy_func);
}

void SymbolTable::set_custom_func(const std::string& name, ComplexFunc func)
{
    set_function(name, func);
}

void SymbolTable::set_custom_func(const std::string& name, ComplexMultiFunc func)
{
    auto found = userFuncTable.find(name);
    auto found2 = funcTable.find(name);
    if (found != end(userFuncTable) || found2 != end(funcTable))
        throw std::runtime_error{ "Cannot override function " + name };
    userFuncTable[name] = func;
}

bool SymbolTable::has_func(const std::string& name) const
{
    return mps::stl::contains(funcTable, name);
}

bool SymbolTable::has_user_func(const std::string & name) const
{
    return mps::stl::contains(userFuncTable, name);
}

void SymbolTable::erase_func(const std::string& name)
{
    auto found = funcTable.find(name);
    assert(found != end(funcTable));
    funcTable.erase(found);
}

void SymbolTable::set_function(const std::string& name, ComplexFunc func)
{
    auto found = funcTable.find(name);
    if (found != end(funcTable))
        throw std::runtime_error{ "Cannot override built-in function " + name };
    funcTable[name] = func;
}

void SymbolTable::add_constants()
{
    set_const("i", { 0, 1 });
    set_const("pi", std::acos(-1));
    set_const("e", 2.7182818284590452354);
    set_const("q_e", 1.60217733e-19);
    set_const("c", 2.99792458e8);
    set_const("m_e", 9.1093897e-31);
}

// https://stackoverflow.com/questions/12500411/setting-a-stdfunction-variable-to-refer-to-the-stdsin-function
#define MAKE_FUNC(f) [] (const Complex& c) { return (f)(c); }
#define MAKE_REAL_FUNC(f) [] (double d) { return (f)(d); }

void SymbolTable::add_default_funcs()
{
    funcTable["sin"] = MAKE_FUNC(std::sin);
    funcTable["cos"] = MAKE_FUNC(std::cos);
    funcTable["tan"] = MAKE_FUNC(std::tan);
    funcTable["asin"] = MAKE_FUNC(std::asin);
    funcTable["acos"] = MAKE_FUNC(std::acos);
    funcTable["atan"] = MAKE_FUNC(std::atan);
    funcTable["sinh"] = MAKE_FUNC(std::sinh);
    funcTable["cosh"] = MAKE_FUNC(std::cosh);
    funcTable["tanh"] = MAKE_FUNC(std::tanh);

    funcTable["abs"] = MAKE_FUNC(std::abs);
    funcTable["norm"] = MAKE_FUNC(std::norm);
    funcTable["arg"] = MAKE_FUNC(std::arg);
    funcTable["exp"] = MAKE_FUNC(std::exp);

    funcTable["sqrt"] = MAKE_FUNC(std::sqrt);
    funcTable["ln"] = MAKE_FUNC(std::log);
    funcTable["log"] = MAKE_FUNC(std::log10);

    add_builtin_real_func("floor", MAKE_REAL_FUNC(std::floor));
    add_builtin_real_func("ceil", MAKE_REAL_FUNC(std::ceil));
    add_builtin_real_func("round", MAKE_REAL_FUNC(std::round));
    add_builtin_real_func("trunc", MAKE_REAL_FUNC(std::trunc));
}

#undef MAKE_FUNC
#undef MAKE_REAL_FUNC
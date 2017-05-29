#include "SymbolTable.hpp"

#include <cassert>
#include <stdexcept>

#include <mps/stl_util.hpp>

Complex SymbolTable::value(const std::string& var)
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

SymbolTable::Var SymbolTable::get_var(const std::string& name) const
{
    auto found = varTable.find(name);
    if (found == end(varTable))
        throw std::runtime_error{ name + " is not defined" };
    return found->second;
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

Complex SymbolTable::value(const std::string& name, Complex arg)
{
    auto found = funcTable.find(name);
    if (found == end(funcTable))
        throw std::runtime_error{ name + " is undefined." };
    return found->second(arg);
}

Complex SymbolTable::value(const std::string & name, const Args & args)
{
    auto found = userFuncTable.find(name);
    if (found == end(userFuncTable))
        throw std::runtime_error{ name + " is undefined." };
    return found->second(args);
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
}

#define MAKE_FUNC(f) [] (const Complex& c) { return (f)(c); }

#define MAKE_FUNC_COMPLEX(f) \
    [] (const Complex& c) { \
        if (c.imag()) \
            throw std::runtime_error{ "Function " #f " not defined for complex numbers" }; \
        return Complex{ (f)(c.real()) }; \
    }

#define MAKE_REAL_FUNC(f) [] (double d) { return (f)(d); }

void SymbolTable::add_default_funcs()
{
    using namespace std;

    funcTable["sin"] = MAKE_FUNC(sin);
    funcTable["cos"] = MAKE_FUNC(cos);
    funcTable["tan"] = MAKE_FUNC(tan);
    funcTable["asin"] = MAKE_FUNC(asin);
    funcTable["acos"] = MAKE_FUNC(acos);
    funcTable["atan"] = MAKE_FUNC(atan);
    funcTable["sinh"] = MAKE_FUNC(sinh);
    funcTable["cosh"] = MAKE_FUNC(cosh);
    funcTable["tanh"] = MAKE_FUNC(tanh);

    funcTable["abs"] = MAKE_FUNC(abs);
    funcTable["norm"] = MAKE_FUNC(norm);
    funcTable["arg"] = MAKE_FUNC(arg);
    funcTable["exp"] = MAKE_FUNC(exp);

    funcTable["sqrt"] = MAKE_FUNC(sqrt);
    funcTable["ln"] = MAKE_FUNC(log);
    funcTable["log"] = MAKE_FUNC(log10);

    funcTable["floor"] = MAKE_FUNC_COMPLEX(floor);
    funcTable["ceil"] = MAKE_FUNC_COMPLEX(ceil);
    funcTable["round"] = MAKE_FUNC_COMPLEX(round);
    funcTable["trunc"] = MAKE_FUNC_COMPLEX(trunc);
}

#undef MAKE_FUNC
#undef MAKE_REAL_FUNC
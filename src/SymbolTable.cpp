#include "SymbolTable.hpp"

#include <cassert>
#include <iostream>
#include <numeric>
#include <stdexcept>

#include <mps/stl_util.hpp>

#include "math_util.hpp"

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

bool SymbolTable::has_list(const std::string& name) const
{
    return mps::stl::contains(listTable, name);
}

bool SymbolTable::has_list_func(const std::string& name) const
{
    return mps::stl::contains(listFuncTable, name);
}

void SymbolTable::set_list(const std::string& name, std::vector<Complex>&& list)
{
    listTable[name] = std::move(list);
}

Complex SymbolTable::call_list_func_with(const std::string& func, const std::string& list)
{
    if (!has_list(list))
        throw std::runtime_error{ "List " + list + " is not defined" };
    if (!has_list_func(func))
        throw std::runtime_error{ "List Function " + func + " is not defined" };
    return listFuncTable[func](listTable[list]);
}

void SymbolTable::add_constants()
{
    set_const("i", { 0, 1 });
    set_const("pi", pi);
    set_const("e", 2.7182818284590452354);
}

#define MAKE_FUNC(f) [] (const auto& c) { return (f)(c); }

#define MAKE_PROXY_FUNC(f) \
    [] (const Complex& c) { \
        if (c.imag()) \
            throw std::runtime_error{ "Function " #f " not defined for complex numbers" }; \
        return Complex{ (f)(c.real()) }; \
    }

void SymbolTable::add_trig_funcs()
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
    funcTable["asinh"] = MAKE_FUNC(std::asinh);
    funcTable["acosh"] = MAKE_FUNC(std::acosh);
    funcTable["atanh"] = MAKE_FUNC(std::atanh);

    using namespace std;
    funcTable["deg"] = MAKE_PROXY_FUNC(deg);
    funcTable["rad"] = MAKE_PROXY_FUNC(rad);
}

void SymbolTable::add_temp_conversions()
{
    using namespace temp;
    funcTable["CtoK"] = MAKE_PROXY_FUNC(CtoK);
    funcTable["KtoC"] = MAKE_PROXY_FUNC(KtoC);
    funcTable["FtoC"] = MAKE_PROXY_FUNC(FtoC);
    funcTable["CtoF"] = MAKE_PROXY_FUNC(CtoF);
    funcTable["FtoK"] = MAKE_PROXY_FUNC(FtoK);
    funcTable["KtoF"] = MAKE_PROXY_FUNC(KtoF);
}

void SymbolTable::add_default_funcs()
{
    add_trig_funcs();
    add_temp_conversions();

    funcTable["abs"] = MAKE_FUNC(std::abs);
    funcTable["norm"] = MAKE_FUNC(std::norm);
    funcTable["arg"] = MAKE_FUNC(std::arg);
    funcTable["exp"] = MAKE_FUNC(std::exp);

    funcTable["sqr"] = [] (auto n) { return n*n; };
    funcTable["cb"] = [] (auto n) { return n*n*n; };
    funcTable["sqrt"] = MAKE_FUNC(std::sqrt);
    funcTable["ln"] = MAKE_FUNC(std::log);
    funcTable["log"] = MAKE_FUNC(std::log10);

    funcTable["Re"] = MAKE_FUNC(std::real);
    funcTable["Im"] = MAKE_FUNC(std::imag);

    listFuncTable["sum"] = [] (const ComplexList& list) { return sum(list); };
    listFuncTable["avg"] = [] (const ComplexList& list) { return avg(list); };
    listFuncTable["len"] = [](const ComplexList& list) { return len(list); };
    listFuncTable["sx"] = [](const ComplexList& list) { return standard_deviation(list); };
    listFuncTable["ux"] = [](const ComplexList& list) { return standard_deviation(list) / std::sqrt(static_cast<double>(len(list))); };

    using namespace std;
    funcTable["floor"] = MAKE_PROXY_FUNC(floor);
    funcTable["ceil"] = MAKE_PROXY_FUNC(ceil);
    funcTable["round"] = MAKE_PROXY_FUNC(round);
    funcTable["trunc"] = MAKE_PROXY_FUNC(trunc);

    funcTable["cbrt"] = MAKE_PROXY_FUNC(cbrt);
}

#undef MAKE_FUNC
#undef MAKE_REAL_FUNC
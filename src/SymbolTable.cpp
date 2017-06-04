#include "SymbolTable.hpp"

#include <cassert>
#include <iostream>
#include <numeric>
#include <stdexcept>

#include <mps/stl_util.hpp>

#include "math_util.hpp"

using namespace std;
using namespace temp;
using namespace mps::stl;

#define CHECK_SINGLE_ARG(f) \
    if (list.size() != 1) throw std::runtime_error{ #f " expects exactly 1 argument" };

#define MAKE_COMPLEX_FUNC(f) [] (const List& list) { \
    CHECK_SINGLE_ARG(f) \
    return (f)(list.front()); }

#define MAKE_PROXY_FUNC(f) [] (const Complex& c) { \
        if (c.imag()) \
            throw std::runtime_error{ #f " not defined for complex numbers" }; \
        return Complex{ (f)(c.real()) }; }

#define MAKE_REAL_FUNC(f) [] (const List& list) { \
    CHECK_SINGLE_ARG(f) \
    if (list.front().imag()) \
        throw std::runtime_error{ #f " not defined for complex numbers" }; \
    return Complex{ (f)(list.front().real()) }; }

SymbolTable::SymbolTable()
    : funcTable{ defaultFuncTable }{ }

bool SymbolTable::is_reserved_func(const std::string& name) const
{
    return contains(defaultFuncTable, name);
}
void SymbolTable::set_var(ConstStrRef name, Complex val)
{
    varTable[name] = val;
}

void SymbolTable::set_list(ConstStrRef name, List&& list)
{
    listTable[name] = list;
}

void SymbolTable::add_func(ConstStrRef name, Func func)
{
    insert_unique(funcTable, name, func);
}

Complex SymbolTable::value_of(ConstStrRef var) const
{
    return find_or_throw(varTable, var, "Variable " + var + " is undefined")->second;
}

const List& SymbolTable::list(ConstStrRef name) const
{
    return find_or_throw(listTable, name, "List " + name + " is undefined")->second;
}

Complex SymbolTable::call_func(ConstStrRef func, const List& arg) const
{
    return find_or_throw(funcTable, func, "Function " + func + " is undefined")->second(arg);
}

bool SymbolTable::has_var(ConstStrRef name) const
{
    return contains(varTable, name);
}

bool SymbolTable::has_list(ConstStrRef name) const
{
    return contains(listTable, name);
}

bool SymbolTable::has_func(ConstStrRef name) const
{
    return contains(funcTable, name);
}

bool SymbolTable::isset(ConstStrRef name) const
{
    return has_var(name) || has_list(name) || has_func(name);
}

void SymbolTable::remove_var(ConstStrRef name)
{
    varTable.erase(name);
}

void SymbolTable::remove_list(ConstStrRef name)
{
    listTable.erase(name);
}

void SymbolTable::remove_func(ConstStrRef name)
{
    funcTable.erase(name);
}

void SymbolTable::remove_symbol(ConstStrRef name)
{
    if (!isset(name))
        throw runtime_error{ name + " is undefined" };
    if (has_func(name))
        remove_func(name);
    if (has_list(name))
        remove_list(name);
    if (has_var(name))
        remove_var(name);
}

void SymbolTable::add_constants()
{
    varTable["i"] = { 0, 1 };
    varTable["pi"] = pi;
    varTable["e"] = 2.7182818284590452354;
}

const std::map<std::string, Func> SymbolTable::defaultFuncTable{
    { "sin", MAKE_COMPLEX_FUNC(sin) },
    { "cos", MAKE_COMPLEX_FUNC(cos) },
    { "tan", MAKE_COMPLEX_FUNC(tan) },
    { "asin", MAKE_COMPLEX_FUNC(asin) },
    { "acos", MAKE_COMPLEX_FUNC(acos) },
    { "atan", MAKE_COMPLEX_FUNC(atan) },
    { "sinh", MAKE_COMPLEX_FUNC(sinh) },
    { "cosh", MAKE_COMPLEX_FUNC(cosh) },
    { "tanh", MAKE_COMPLEX_FUNC(tanh) },
    { "asinh", MAKE_COMPLEX_FUNC(asinh) },
    { "acosh", MAKE_COMPLEX_FUNC(acosh) },
    { "atanh", MAKE_COMPLEX_FUNC(atanh) },
    { "deg", MAKE_REAL_FUNC(deg) },
    { "rad", MAKE_REAL_FUNC(rad) },

    { "CtoK", MAKE_REAL_FUNC(CtoK) },
    { "KtoC", MAKE_REAL_FUNC(KtoC) },
    { "FtoC", MAKE_REAL_FUNC(FtoC) },
    { "CtoF", MAKE_REAL_FUNC(CtoF) },
    { "FtoK", MAKE_REAL_FUNC(FtoK) },
    { "KtoF", MAKE_REAL_FUNC(KtoF) },

    { "abs", MAKE_COMPLEX_FUNC(abs) },
    { "norm", MAKE_COMPLEX_FUNC(norm) },
    { "arg", MAKE_COMPLEX_FUNC(arg) },
    { "exp", MAKE_COMPLEX_FUNC(exp) },

    { "sqr", MAKE_COMPLEX_FUNC(sqr) },
    { "sqrt", MAKE_COMPLEX_FUNC(sqrt) },
    { "ln", MAKE_COMPLEX_FUNC(log) },
    { "log", MAKE_COMPLEX_FUNC(log10) },

    { "Re", MAKE_COMPLEX_FUNC(real) },
    { "Im", MAKE_COMPLEX_FUNC(imag) },

    { "floor", MAKE_REAL_FUNC(floor) },
    { "ceil", MAKE_REAL_FUNC(ceil) },
    { "round", MAKE_REAL_FUNC(round) },
    { "trunc", MAKE_REAL_FUNC(trunc) },

    { "cbrt", MAKE_REAL_FUNC(cbrt) },

    { "sum", sum },
    { "sum2", sqr_sum },
    { "avg", avg },
    { "len", len },
    { "sx", standard_deviation },
    { "ux", standard_uncertainty }
};
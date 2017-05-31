#include "SymbolTable.hpp"

#include <cassert>
#include <iostream>
#include <numeric>
#include <stdexcept>

#include <mps/stl_util.hpp>

#include "math_util.hpp"

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

void SymbolTable::set_var(ConstStrRef name, Complex val)
{
    auto found = varTable.find(name);
    if (found == cend(varTable) || !found->second.isConst)
        varTable[name] = make_var(val);
    else
        throw std::runtime_error{ "Cannot override constant " + name };
}

void SymbolTable::set_list(ConstStrRef name, List&& list)
{
    listTable[name] = list;
}

void SymbolTable::add_const(ConstStrRef name, Complex val)
{
    insert_unique(varTable, name, make_const(val));
}

void SymbolTable::set_func(ConstStrRef name, Func func)
{
    funcTable[name] = func;
}

Complex SymbolTable::value_of(ConstStrRef var) const
{
    return find_or_throw(varTable, var, "Variable " + var + " is undefined")->second.val;
}

const List& SymbolTable::list(ConstStrRef name) const
{
    return find_or_throw(listTable, name, "List " + name + " is undefined")->second;
}

Complex SymbolTable::call_func(ConstStrRef func, const List& arg) const
{
    return find_or_throw(funcTable, func, "Function " + func + " is undefined")->second(arg);
}

bool SymbolTable::is_const(ConstStrRef name) const
{
    return contains_if(varTable, name, [](const auto& item) { return item.isConst; });
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

void SymbolTable::add_constants()
{
    add_const("i", { 0, 1 });
    add_const("pi", pi);
    add_const("e", 2.7182818284590452354);
}

void SymbolTable::add_trig_funcs()
{
    using namespace std;

    funcTable["sin"] = MAKE_COMPLEX_FUNC(sin);
    funcTable["cos"] = MAKE_COMPLEX_FUNC(cos);
    funcTable["tan"] = MAKE_COMPLEX_FUNC(tan);
    funcTable["asin"] = MAKE_COMPLEX_FUNC(asin);
    funcTable["acos"] = MAKE_COMPLEX_FUNC(acos);
    funcTable["atan"] = MAKE_COMPLEX_FUNC(atan);
    funcTable["sinh"] = MAKE_COMPLEX_FUNC(sinh);
    funcTable["cosh"] = MAKE_COMPLEX_FUNC(cosh);
    funcTable["tanh"] = MAKE_COMPLEX_FUNC(tanh);
    funcTable["asinh"] = MAKE_COMPLEX_FUNC(asinh);
    funcTable["acosh"] = MAKE_COMPLEX_FUNC(acosh);
    funcTable["atanh"] = MAKE_COMPLEX_FUNC(atanh);

    funcTable["deg"] = MAKE_REAL_FUNC(deg);
    funcTable["rad"] = MAKE_REAL_FUNC(rad);
}

void SymbolTable::add_temp_conversions()
{
    using namespace temp;
    funcTable["CtoK"] = MAKE_REAL_FUNC(CtoK);
    funcTable["KtoC"] = MAKE_REAL_FUNC(KtoC);
    funcTable["FtoC"] = MAKE_REAL_FUNC(FtoC);
    funcTable["CtoF"] = MAKE_REAL_FUNC(CtoF);
    funcTable["FtoK"] = MAKE_REAL_FUNC(FtoK);
    funcTable["KtoF"] = MAKE_REAL_FUNC(KtoF);
}

void SymbolTable::add_list_funcs()
{
    funcTable["sum"] = sum;
    funcTable["sum2"] = sqr_sum;
    funcTable["avg"] = avg;
    funcTable["len"] = len;
    funcTable["sx"] = standard_deviation;
    funcTable["ux"] = [] (const List& list) { 
        return standard_deviation(list) / std::sqrt(static_cast<double>(len(list))); 
    };
}

void SymbolTable::add_default_funcs()
{
    add_trig_funcs();
    add_temp_conversions();
    add_list_funcs();

    using namespace std;

    funcTable["abs"] = MAKE_COMPLEX_FUNC(abs);
    funcTable["norm"] = MAKE_COMPLEX_FUNC(norm);
    funcTable["arg"] = MAKE_COMPLEX_FUNC(arg);
    funcTable["exp"] = MAKE_COMPLEX_FUNC(exp);

    funcTable["sqr"] = MAKE_COMPLEX_FUNC(sqr);
    funcTable["sqrt"] = MAKE_COMPLEX_FUNC(sqrt);
    funcTable["ln"] = MAKE_COMPLEX_FUNC(log);
    funcTable["log"] = MAKE_COMPLEX_FUNC(log10);

    funcTable["Re"] = MAKE_COMPLEX_FUNC(real);
    funcTable["Im"] = MAKE_COMPLEX_FUNC(imag);

    funcTable["floor"] = MAKE_REAL_FUNC(floor);
    funcTable["ceil"] = MAKE_REAL_FUNC(ceil);
    funcTable["round"] = MAKE_REAL_FUNC(round);
    funcTable["trunc"] = MAKE_REAL_FUNC(trunc);

    funcTable["cbrt"] = MAKE_REAL_FUNC(cbrt);
}

SymbolTable::Var make_var(Complex val)
{
    return { std::move(val), false };
}

SymbolTable::Var make_const(Complex val)
{
    return { std::move(val), true };
}
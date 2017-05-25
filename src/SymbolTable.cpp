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
	return found->second.func(arg);
}

void SymbolTable::add_builtin_func(const std::string& name, ComplexFunc func)
{
	set_function(name, { func, Func::BuiltIn });
}

void SymbolTable::add_builtin_real_func(const std::string& name, std::function<double(double)> func)
{
	auto proxy_func = [f = std::move(func), name](const Complex& c) { 
		if (c.imag()) {
			throw std::runtime_error{ name + " not defined for complex numbers" };
		}
		return Complex{ f(c.real()), 0 };	
	};
	set_function(name, { proxy_func, Func::BuiltIn });
}

void SymbolTable::set_custom_func(const std::string& name, ComplexFunc func)
{
	set_function(name, { func, Func::Custom });
}

bool SymbolTable::has_func(const std::string& name) const
{
	return mps::stl::contains(funcTable, name);
}

void SymbolTable::erase_func(const std::string& name)
{
	auto found = funcTable.find(name);
	assert(found != end(funcTable));
	funcTable.erase(found);
}

void SymbolTable::set_function(const std::string& name, Func func)
{
	auto found = funcTable.find(name);
	if (found != end(funcTable) && found->second.type == Func::BuiltIn)
		throw std::runtime_error{ "Cannot override built-in function " + name };
	funcTable[name] = func;
}

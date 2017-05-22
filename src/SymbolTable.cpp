#include "SymbolTable.hpp"

#include <cassert>

#include <mps/stl_util.hpp>

SymbolTable::Complex SymbolTable::value_of(const std::string& var)
{
	auto found = varTable.find(var);
	if (found == end(varTable))
		throw std::runtime_error{ var + " is undefined." };
	return found->second.value;
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


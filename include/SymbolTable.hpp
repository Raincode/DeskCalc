#pragma once

#include <complex>
#include <map>
#include <string>

class SymbolTable {
public:
	using Complex = std::complex<double>;
	struct Var {
		enum Type { Mutable, Const };
		Complex value;
		Type type{};
	};

	SymbolTable() = default;

	Complex value_of(const std::string& var);
	void set_var(const std::string& name, Complex value);
	void set_const(const std::string& name, Complex value);
	bool has_var(const std::string& name) const;
	void erase_var(const std::string& name);

private:
	void set_value(const std::string& name, Var var);

	std::map<std::string, Var> varTable;
};
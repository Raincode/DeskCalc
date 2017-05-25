#pragma once

#include <complex>
#include <functional>
#include <map>
#include <string>

using Complex = std::complex<double>;
using ComplexFunc = std::function<Complex(const Complex&)>;

class SymbolTable {
public:
	struct Var {
		enum Type { Mutable, Const };
		Complex value;
		Type type{};
	};

	struct Func {
		enum Type { Custom, BuiltIn };
		ComplexFunc func;
		Type type;
	};


	SymbolTable() = default;

	Complex value_of(const std::string& var);
	void set_var(const std::string& name, Complex value);
	void set_const(const std::string& name, Complex value);
	bool has_var(const std::string& name) const;
	void erase_var(const std::string& name);

	Complex call_func(const std::string& name, Complex arg);
	void add_builtin_func(const std::string& name, ComplexFunc func);
	void add_builtin_real_func(const std::string& name, std::function<double(double)> func);
	void set_custom_func(const std::string& name, ComplexFunc func);
	bool has_func(const std::string& name) const;
	void erase_func(const std::string& name);

private:
	void set_value(const std::string& name, Var var);
	void set_function(const std::string& name, Func func);

	std::map<std::string, Var> varTable;
	std::map<std::string, Func> funcTable;
};
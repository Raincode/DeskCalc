#pragma once

#include <complex>
#include <istream>
#include <map>
#include <string>

#include "Lexer.hpp"
#include "SymbolTable.hpp"

using Complex = std::complex<double>;

class Parser {
public:
	explicit Parser(Lexer::TokenStream& lexer);

	void parse();
	void parse(const std::string& input);
	Complex result() const { return res; }
	SymbolTable& symbol_table() { return table; }

private:
	Complex expr();
	Complex term();
	Complex pow();
	Complex prim();

	bool consume(Kind kind);
	bool peek(Kind kind) const;
	void expect(Kind kind);

	void check_all_paren_closed();
	void check_open_paren();
	void consume_closing_paren();

	Token cachedToken;
	SymbolTable table;
	Lexer::TokenStream& ts;
	int lparenCount{};
	Complex res;
};

Complex safe_div(Complex left, Complex right);
unsigned factorial(int n);


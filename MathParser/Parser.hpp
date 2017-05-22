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
	Complex result() const { return res; }

private:
	Complex expr();
	Complex term();
	Complex prim();

	bool consume(Kind kind);
	bool peek(Kind kind) const;
	void expect(Kind kind);

	Token cachedToken;
	SymbolTable table;
	Lexer::TokenStream& ts;
	int lparenCount{};
	Complex res;
};

Complex safe_div(Complex left, Complex right);


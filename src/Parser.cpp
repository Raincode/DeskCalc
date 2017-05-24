#include "Parser.hpp"

#include <cmath>
#include <iostream>

#include <mps/stl_util.hpp>
#include <mps/str_util.hpp>

#include "Warning.hpp"

Parser::Parser(Lexer::TokenStream& ts)
	: ts{ ts }
{
	table.set_const("i", {0, 1});
	table.set_const("pi", 3.1415926535897932385);
	table.set_const("e", 2.7182818284590452354);
}

void Parser::parse()
{
	ts.get();
	res = expr();
}

void Parser::parse(const std::string& str)
{
	ts.set_input(str);
	parse();
}

Complex Parser::expr()
{
	auto left = term();
	for (;;) {
		if (consume(Kind::Plus)) {
			left += term();
		}
		else if (consume(Kind::Minus)) {
			left -= term();
		}
		else if (peek(Kind::RParen)) {
			consume_closing_paren();
			if (consume(Kind::String)) {
				return left * table.value_of(cachedToken.str);
			}
			return left;
		}
		else if (consume(Kind::Print) || consume(Kind::End)) {
			check_all_paren_closed();
			return left;
		}
		else {
			throw std::runtime_error{ "Syntax error" };
		}
	}
}

Complex Parser::term()
{
	auto left = pow();
	for (;;) {
		if (consume(Kind::Mul)) {
			left *= pow();
		}
		else if (consume(Kind::Div)) {
			left = safe_div(left, pow());
		}
		else if (consume(Kind::Parallel)) {
			auto right = pow();
			if ((left.imag() && right.imag()) ||
				(left.real() > 0 && right.real() > 0)) {
				left = left * right / (left + right);
			}
			else {
				throw std::runtime_error{ "Resistors must be greater than 0" };
			}
		}
		else {
			return left;
		}
	}
}

unsigned factorial(int n)
{
	if (n < 0) {
		throw std::runtime_error{ "Factorial not defined for negative numbers" };
	}
	else if (n == 1) {
		return 0;
	}
	else {
		unsigned result{ 1 };
		for (int i = 2; i <= n; ++i) {
			result *= i;
		}
		return result;
	}
}

Complex Parser::pow()
{
	auto left = prim();
	for (;;) {
		if (consume(Kind::Pow)) {
			return std::pow(left, pow());
		}
		else if (consume(Kind::Fac)) {
			static Warning w{ "Note: Factorial might overflow for larger numbers" };
			double trunc = std::trunc(left.real());
			if (!left.imag() && trunc == left.real() && trunc >= 0) {
				return factorial(static_cast<int>(trunc));
			}
			throw std::runtime_error{ "Factorial only defined for N and 0" };
		}
		else {
			return left;
		}
	}
}

Complex Parser::prim()
{
	if (consume(Kind::Number)) {
		auto num = cachedToken.num;
		if (consume(Kind::String)) {
			return num * table.value_of(cachedToken.str);
		}
		return num;
	}
	else if (consume(Kind::String)) {
		auto str = cachedToken.str;
		if (consume(Kind::Assign)) {
			table.set_var(str, expr());
		}
		return table.value_of(str);
	}
	else if (consume(Kind::LParen)) {
		++lparenCount;
		auto val = expr();
		return val;
	}
	else if (consume(Kind::Minus)) {
		return -prim();
	}
	throw std::runtime_error{ "Unexpected Token " + mps::str::to_string(ts.current()) };
}

void Parser::check_open_paren()
{
	if (!lparenCount) {
		lparenCount = 0;
		throw std::runtime_error{ "Unexpected Token: )" };
	}
}

void Parser::check_all_paren_closed()
{
	if (lparenCount) {
		lparenCount = 0;
		throw std::runtime_error{ "Expected Token: )" };
	}
}

void Parser::consume_closing_paren()
{
	while (consume(Kind::RParen)) {
		check_open_paren();
		--lparenCount;
	}
}

bool Parser::consume(Kind kind)
{
	if (ts.current().kind == kind) {
		cachedToken = ts.current();
		ts.get();
		return true;
	}
	return false;
}

bool Parser::peek(Kind kind) const
{
	return ts.current().kind == kind;
}

void Parser::expect(Kind kind)
{
	if (!consume(kind)) {
		throw std::runtime_error{ "Expected Token " + mps::str::to_string(Token{kind}) };
	}
}

Complex safe_div(Complex left, Complex right)
{
	if (right.real() || right.imag()) {
		return left / right;
	}
	throw std::runtime_error{ "Divide by zero" };
}
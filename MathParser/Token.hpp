#pragma once

#include <string>

enum class Kind : char {
	End,
	Number,
	String,
	Parallel,
	Print = '\n',

	Plus = '+', Minus = '-', Mul = '*', Div = '/',
	FloorDiv,
	Mod = '%',
	Pow = '^',
	Fac = '!',
	Assign = '=',
	Comma = ',',

	LParen = '(', RParen = ')',
	LBrace = '{', RBrace = '}',
	LBracket = '[', RBracket = ']',

	Invalid
};

struct Token {
	Kind kind{};
	std::string str;
	double num;
};

inline std::ostream& operator<<(std::ostream& os, const Token& token)
{
	switch (token.kind) {
	case Kind::Number:
		return os << token.num;
	case Kind::String:
		return os << token.str;
	case Kind::Print:
		return os << "PRINT";
	case Kind::End:
		return os << "END";
	default:
		return os << "OPERATOR " << static_cast<char>(token.kind);
	}
}
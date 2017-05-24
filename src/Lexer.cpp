#include "Lexer.hpp"

#include <cassert>
#include <cctype>
#include <iomanip>
#include <sstream>

namespace Lexer {
	TokenStream::TokenStream()
		: input{}, ownsInput{}
	{
	}

	TokenStream::TokenStream(std::istream& is)
		: input{ &is }, ownsInput{}
	{
	}

	TokenStream::TokenStream(std::istream* is)
		: input{ is }, ownsInput{ true }
	{
	}

	TokenStream::TokenStream(const std::string& str)
		: input{ new std::istringstream{str} }, ownsInput{ true }
	{

	}

	void TokenStream::set_input(std::istream& is)
	{
		cleanup();
		input = &is;
	}

	void TokenStream::set_input(std::istream* is)
	{
		cleanup();
		input = is;
		ownsInput = true;
	}

	void TokenStream::set_input(const std::string& str)
	{
		set_input(new std::istringstream{ str });
	}

	Token TokenStream::get()
	{
		char ch;
		do {
			if (!input || !input->get(ch)) {
				return ct = { Kind::End };
			}
		} while (std::isspace(static_cast<unsigned char>(ch)) && ch != '\n');

		switch (ch) {
		case '\n':
		case '+':
		case '-':
		case '%':
		case '!':
		case '^':
		case ',':
		case '=':
		case '(': case ')':
		case '[': case ']':
		case '{': case '}':
			return ct = { static_cast<Kind>(ch) };
		case '*':
			return ct = parse_double_op('*', Kind::Pow, Kind::Mul);
		case '/':
			return ct = parse_double_op('/', Kind::FloorDiv, Kind::Div);
		case '|':
			return ct = parse_double_op('|', Kind::Parallel, Kind::Invalid);
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		case '.':
			input->unget();
			*input >> ct.num;
			ct.kind = Kind::Number;
			return ct;
		default:
			return ct = parse_identifier(ch);
		}
		while (ct.kind != Kind::Print) get();
		throw std::runtime_error{ "Bad Token" };
	}

	const Token& TokenStream::current() const
	{
		return ct;
	}

	Token TokenStream::parse_double_op(char second, Kind onMatch, Kind onFailure)
	{
		char ch;
		if (input->get(ch) && ch == second)
			return { onMatch };
		else {
			input->unget();
			if (onFailure != Kind::Invalid)
				return { onFailure };
			throw std::runtime_error{ "Expected Token: " + second };
		}
	}

	Token TokenStream::parse_identifier(char ch)
	{
		if (std::isalpha(static_cast<unsigned char>(ch)) || ch == '_') {
			std::string s{ ch };
			while (input->get(ch) && (std::isalnum(ch) || ch == '_')) {
				s += ch;
			}
			input->unget();
			if (s == "div") return { Kind::FloorDiv };
			if (s == "mod") return { Kind::Mod };
			return { Kind::String, s };
		}
		else {
			throw std::runtime_error{ "Bad Token" };
		}
	}

	void TokenStream::cleanup()
	{
		if (ownsInput) {
			delete input;
			ownsInput = false;
		}
	}
}	/* namespace Lexer */
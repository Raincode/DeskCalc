#pragma once

#include <istream>

#include "Token.hpp"

namespace Lexer {
	class TokenStream {
	public:
		TokenStream();
		explicit TokenStream(std::istream& input);
		explicit TokenStream(std::istream* input);
		explicit TokenStream(const std::string& input);

		TokenStream(const TokenStream&) = delete;
		TokenStream& operator=(const TokenStream&) = delete;

		void set_input(std::istream& input);
		void set_input(std::istream* input);
		void set_input(const std::string& input);

		Token get();
		const Token& current() const;

	private:
		void cleanup();
		Token parse_identifier(char firstChar);
		Token parse_double_op(char expected, Kind onSuccess, Kind onFailure);

		Token ct{ Kind::End };
		std::istream* input;
		bool ownsInput;
	};
}	/* namespace Lexer */


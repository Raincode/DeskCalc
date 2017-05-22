#include "catch.hpp"

#include "Parser.hpp"

TEST_CASE("Parser Test", "[Parser]") {
	Lexer::TokenStream ts;
	Parser parser{ ts };

	parser.parse("1 + 2*(3 + 4) + 5*pi - pi*e / (1 + 27*2 + 3) - 5");
	REQUIRE((parser.result().real() - 25.5607) < 10e-3);
	REQUIRE(parser.result().imag() == 0);

	parser.symbol_table().set_const("a", 42);
	parser.parse("10a");
	REQUIRE(parser.result() == Complex{ 420 });

	parser.parse("17i - 5 * (2+3i) - (3i * i - 5 * 3 / (((i))))");
	REQUIRE(parser.result() == Complex(-7, -13));

	REQUIRE_THROWS(parser.parse("(5"));
	REQUIRE_THROWS(parser.parse("5)"));
	REQUIRE_NOTHROW(parser.parse("(((5)))"));
}
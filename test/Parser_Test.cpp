#include "catch.hpp"

#include "Parser.hpp"

TEST_CASE("Parser Test", "[Parser]") {
    SymbolTable table;
    table.add_default_funcs();
    table.add_constants();
    Parser parser{ table };

    REQUIRE_NOTHROW(parser.parse("1 + 2*(3 + 4) + 5*pi - pi*e / (1 + 27*2 + 3) - 5"));
    REQUIRE((parser.result().real() - 25.5607) < 10e-3);
    REQUIRE(parser.result().imag() == 0);

    parser.symbol_table().add_const("a", 42);
    parser.parse("10a");
    REQUIRE(parser.result() == Complex{ 420 });

    parser.parse("17i - 5 * (2+3i) - (3i * i - 5 * 3 / (((i))))");
    REQUIRE(parser.result() == Complex(-7, -13));

    REQUIRE_NOTHROW(parser.parse("+5"));
    REQUIRE(parser.result() == Complex(5, 0));

    parser.parse("-5");
    REQUIRE(parser.result() == Complex(-5, 0));

    parser.parse("i");
    REQUIRE(parser.result() == Complex(0, 1));

    parser.parse("x = 5");
    REQUIRE_NOTHROW(parser.symbol_table().value_of("x"));
    REQUIRE(parser.symbol_table().value_of("x") == Complex(5, 0));

    REQUIRE_THROWS(parser.parse("(5"));
    REQUIRE_THROWS(parser.parse("5)"));
    REQUIRE_NOTHROW(parser.parse("(((5)))"));

    REQUIRE_NOTHROW(parser.parse("fn f(a,b,c) = a + b + c"));
    REQUIRE_NOTHROW(parser.parse("f(42,42,1)"));
    REQUIRE(parser.has_result());
    REQUIRE(parser.result() == Complex(85));

    REQUIRE(parser.symbol_table().value_of("a") == Complex(42));
    REQUIRE_THROWS(parser.parse("a = 0"));
}
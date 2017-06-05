#include "catch.hpp"

#include "Parser.hpp"
#include "SymbolTable.hpp"

#define REQUIRE_RESULT(res) \
    REQUIRE(parser.has_result()); \
    REQUIRE(parser.result() == (res));

#define REQUIRE_PARSE_RESULT(input, res) \
    REQUIRE_NOTHROW(parser.parse(input)); \
    REQUIRE_RESULT(res);

TEST_CASE("Parser Test", "[Parser]") {
    SymbolTable table;
    Parser parser{ table };

    SECTION("Functions") {
        REQUIRE_NOTHROW(parser.parse("fn f(a,b,c) = a + b + c"));
        REQUIRE_FALSE(parser.has_result());
        REQUIRE_NOTHROW(parser.parse("f(42,42,1)"));
        REQUIRE_RESULT(Complex{ 85 });

        REQUIRE_THROWS(parser.parse("undefined(1,2,3)"));
        REQUIRE_THROWS(parser.parse("fn g(1) = 42"));
    }

    SECTION("Expressions") {
        REQUIRE_PARSE_RESULT("42", Complex(42));
        REQUIRE_PARSE_RESULT("+42", Complex(42));
        REQUIRE_PARSE_RESULT("-42", Complex(-42));

        REQUIRE_PARSE_RESULT("1+i", Complex(1, 1));
        REQUIRE_PARSE_RESULT("i", Complex(0, 1));
        REQUIRE_PARSE_RESULT("-i", Complex(0, -1));

        REQUIRE_THROWS(parser.parse("(42"));
        REQUIRE_THROWS(parser.parse("42)"));
        REQUIRE_PARSE_RESULT("(((42)))", Complex(42));

        REQUIRE_PARSE_RESULT("2(3+4)", Complex(14));

        REQUIRE_NOTHROW(parser.parse("5pi^2"));
        REQUIRE(parser.has_result());
        REQUIRE(parser.result().real() - 49.348 < 10e-3);

        REQUIRE_NOTHROW(parser.parse("1 + 2*(3 + 4) + 5*pi - pi*e / (1 + 27*2 + 3) - 5"));
        REQUIRE(parser.result().real() - 25.5607 < 10e-3);  // comparing doubles is nasty business
        REQUIRE(parser.result().imag() == 0);

        REQUIRE_PARSE_RESULT("17i - 5 * (2+3i) - (3i * i - 5 * 3 / (((i))))", Complex(-7, -13));
    }

    SECTION("Variables") {
        REQUIRE_NOTHROW(parser.symbol_table().set_var("a", 42));
        REQUIRE_PARSE_RESULT("10a", Complex{ 420 });

        REQUIRE_NOTHROW(parser.parse("x = 5"));
        REQUIRE_NOTHROW(parser.symbol_table().value_of("x"));
        REQUIRE(parser.symbol_table().value_of("x") == Complex(5, 0));
    }

    SECTION("Lists") {
        REQUIRE_PARSE_RESULT("x = [1, 2, 1, 2, 1, 2]; avg(x)", Complex{ 1.5 });
        REQUIRE_PARSE_RESULT("len(x)", Complex{ 6 });
        REQUIRE_PARSE_RESULT("sum(x)", Complex{ 9 });

        REQUIRE_NOTHROW(parser.parse("sx(x)"));
        REQUIRE(parser.has_result());
        REQUIRE(parser.result().real() - 0.547723 < 10e-3);

        REQUIRE_NOTHROW(parser.parse("ux(x)"));
        REQUIRE(parser.has_result());
        REQUIRE(parser.result().real() - 0.223607 < 10e-3);
    }

    SECTION("Deletions") {
        REQUIRE_NOTHROW(parser.parse("a = 42"));
        REQUIRE_NOTHROW(parser.parse("del a"));
        REQUIRE_FALSE(parser.symbol_table().has_var("a"));
    }
}
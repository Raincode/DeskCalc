#include "catch.hpp"
#include "math_util.hpp"

TEST_CASE("Math Utility Test", "[math_util]") {
    REQUIRE_THROWS(safe_div(1, 0));
    REQUIRE_NOTHROW(safe_div(0, { 0, 1 }));
    REQUIRE(safe_div(256, 2) == Complex(128));

    REQUIRE_THROWS(safe_floordiv(1, 0));
    REQUIRE_NOTHROW(safe_floordiv(0, { 0, 1 }));
    REQUIRE(safe_floordiv(17, 2) == Complex(8));

    REQUIRE_THROWS(safe_mod(1, 0));
    REQUIRE_NOTHROW(safe_mod(0, 1));
    REQUIRE_THROWS(safe_mod(1, { 0, 1 }));
    REQUIRE(safe_mod(10, 3) == 1);

    REQUIRE_THROWS(impedance_parallel(0, 0));
    REQUIRE(impedance_parallel(42, 42) == Complex(21));
    REQUIRE(impedance_parallel(1000, 0) == Complex(0));

    REQUIRE_THROWS(factorial(-1));
    REQUIRE_THROWS(factorial({ 0, 1 }));

    REQUIRE(factorial(0) == 1);
    REQUIRE(factorial(1) == 1);
    REQUIRE(factorial(2) == 2);
    REQUIRE(factorial(3) == 6);
    REQUIRE(factorial({ 4, 0 }) == Complex(24));
    REQUIRE(factorial({ 5, 0 }) == Complex(120));
}
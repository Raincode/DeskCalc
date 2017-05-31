#include "catch.hpp"
#include "SymbolTable.hpp"

#include <cmath>

TEST_CASE("SymbolTable test", "[SymbolTable]") {
    SymbolTable table;

    table.add_const("something", 42);
    REQUIRE(table.has_var("something"));
    REQUIRE(table.value_of("something") == Complex{ 42 });
    REQUIRE_THROWS(table.set_var("something", 0));
    table.remove_var("something");
    REQUIRE_FALSE(table.has_var("something"));

    table.set_var("a", 42);
    REQUIRE(table.has_var("a"));
    REQUIRE(table.value_of("a") == Complex{ 42 });
    table.remove_var("a");
    REQUIRE_FALSE(table.has_var("a"));

    table.set_func("sin", [] (auto n) { return std::sin(n.front()); });
    REQUIRE(table.has_func("sin"));
    REQUIRE(table.call_func("sin", { 0 }) == Complex{ 0 });
    table.remove_func("sin");
    REQUIRE_FALSE(table.has_func("sin"));
}
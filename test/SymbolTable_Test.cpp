#include "catch.hpp"
#include "SymbolTable.hpp"

#include <cmath>

TEST_CASE("SymbolTable test", "[SymbolTable]") {
    SymbolTable table;

    table.set_var("a", 42);
    REQUIRE(table.has_var("a"));
    REQUIRE(table.value_of("a") == Complex{ 42 });
    table.remove_var("a");
    REQUIRE_FALSE(table.has_var("a"));
}
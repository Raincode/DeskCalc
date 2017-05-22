#include "catch.hpp"
#include "SymbolTable.hpp"

TEST_CASE("SymbolTable test", "[SymbolTable]") {
	SymbolTable table;

	table.set_var("a", 42);
	REQUIRE(table.has_var("a"));
	REQUIRE(table.value_of("a") == SymbolTable::Complex{ 42 });
	table.set_var("a", 0);
	REQUIRE(table.value_of("a") == SymbolTable::Complex{ 0 });
	table.erase_var("a");
	REQUIRE(!table.has_var("a"));

	table.set_const("a", 42);
	REQUIRE(table.has_var("a"));
	REQUIRE(table.value_of("a") == SymbolTable::Complex{ 42 });
	REQUIRE_THROWS(table.set_var("a", 0));
	REQUIRE_THROWS(table.set_const("a", 0));
	table.erase_var("a");
	REQUIRE(!table.has_var("a"));
}
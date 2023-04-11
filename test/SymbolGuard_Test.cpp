#include "catch.hpp"
#include "SymbolGuard.hpp"

TEST_CASE("SymbolGuard test", "[SymbolGuard]") {
    SymbolTable table;
	table.set_var("a", 42);
    REQUIRE(table.has_var("a"));
    REQUIRE(table.value_of("a") == Complex{ 42 });
	
	{
		SymbolGuard guard{table};
		guard.shadow_var("a", 0);
		REQUIRE(table.has_var("a"));
		REQUIRE(table.value_of("a") == Complex{ 0 });
	}
    
	REQUIRE(table.has_var("a"));
    REQUIRE(table.value_of("a") == Complex{ 42 });
}
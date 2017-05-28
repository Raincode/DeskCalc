#include "catch.hpp"

#include "TokenStream.hpp"

TEST_CASE("Lexer Test", "[Lexer]") {
    TokenStream ts;
    REQUIRE(ts.current().kind == Kind::End);
    REQUIRE(ts.get().kind == Kind::End);

    ts.set_input("ABCD 1234");
    REQUIRE(ts.get().kind == Kind::String);
    REQUIRE(ts.current().str == "ABCD");
    REQUIRE(ts.get().kind == Kind::Number);
    REQUIRE(ts.current().num == 1234);

    std::string tokstr{ "+-*/%()[]{}^!=," };
    ts.set_input(tokstr);
    for (auto c : tokstr) {
        REQUIRE(ts.get().kind == static_cast<Kind>(c));
    }

    ts.set_input(";\n");
    REQUIRE(ts.get().kind == Kind::Print);
    REQUIRE(ts.get().kind == Kind::Print);

    std::string specialOps{ "** || // mod div" };
    ts.set_input(specialOps);
    REQUIRE(ts.get().kind == Kind::Pow);
    REQUIRE(ts.get().kind == Kind::Parallel);
    REQUIRE(ts.get().kind == Kind::FloorDiv);
    REQUIRE(ts.get().kind == Kind::Mod);
    REQUIRE(ts.get().kind == Kind::FloorDiv);
}
#include "catch.hpp"
#include "mps/clipboard.hpp"

#include <array>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

TEST_CASE("Clipboard Test", "[clipboard]") {
    static const std::array<const char*, 5> inputs{
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",
        "0123456789",
        "!\"§$%&/()=?{[]}\\,;.:-_#'+*~'",
        "Hello,World!",
        "Hello, World !"
    };
    
    for (const auto& i : inputs) {
        mps::set_clipboard_text(i);
        std::this_thread::sleep_for(10ms); // give OS a little time, tests randomly failed without the sleep
        REQUIRE(mps::get_clipboard_text() == i);
        std::this_thread::sleep_for(10ms);
    }
}
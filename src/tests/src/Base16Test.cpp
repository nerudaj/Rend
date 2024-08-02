#include "utils/Base16.hpp"
#include <catch.hpp>

TEST_CASE("[Base16]")
{
    SECTION("Encode works")
    {
        auto str = std::string { "hello" };
        auto encoded = Base16::toBase16(str);
        REQUIRE(encoded == "gigfgmgmgp");
        REQUIRE(Base16::fromBase16(encoded) == str);
    }
}
#include <catch.hpp>
#include "include/Utilities/Utilities.hpp"

TEST_CASE("[Utilities]")
{
	SECTION("unifyRects")
	{
		SECTION("Returns empty optional if both params are empty optionals")
		{
			REQUIRE(!Utilities::unifyRects({}, {}).has_value());
		}

		SECTION("Returns first rect if only second one is empty optional")
		{
			TileRect r(10, 20, 30, 40);
			REQUIRE(Utilities::unifyRects(r, {}) == r);
		}

		SECTION("Returns second rect if only first one is empty optional")
		{
			TileRect r(10, 20, 30, 40);
			REQUIRE(Utilities::unifyRects({}, r) == r);
		}

		SECTION("Properly unifies both valid rectangles")
		{

			TileRect r1(10, 20, 15, 25);
			TileRect r2(30, 40, 35, 45);
			TileRect unified(10, 20, 35, 45);

			auto result = Utilities::unifyRects(r1, r2);
			REQUIRE(result == unified);
		}
	}
}
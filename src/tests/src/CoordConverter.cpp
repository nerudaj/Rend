#include "Utilities/CoordConverter.hpp"
#include <catch.hpp>

TEST_CASE("[CoordConverter]")
{
    SECTION("convertCoordToTileRect")
    {
        CoordConverter cc(sf::Vector2u(32u, 32u));

        CoordRect src { .left = 63, .top = 44, .right = 126, .bottom = 193 };

        TileRect target { .left = 1, .top = 1, .right = 3, .bottom = 6 };

        REQUIRE(cc.convertCoordToTileRect(src) == target);
    }
}

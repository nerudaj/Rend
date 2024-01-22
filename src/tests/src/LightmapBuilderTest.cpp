#include "builder/LightmapBuilder.hpp"
#include <catch.hpp>

LevelD prepareLevel(unsigned layerWidth, unsigned layerHeight)
{
    auto level = LevelD {};
    level.mesh.layerWidth = layerWidth;
    level.mesh.layerHeight = layerHeight;
    level.mesh.tileWidth = 16;
    level.mesh.tileHeight = 16;
    level.mesh.layers.resize(2);
    return level;
}

TEST_CASE("[LightmapBuilder]")
{
    SECTION("Correct vertical light decay")
    {
        auto&& level = prepareLevel(3u, 3u);
        level.mesh.layers[0].tiles = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        level.mesh.layers[0].blocks = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        // sky light in the middle
        level.mesh.layers[1].tiles = { 0, 0, 0, 0, 3, 0, 0, 0, 0 };
        level.mesh.layers[1].blocks = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

        auto lightmap =
            LightmapBuilder::buildLightmap(level, SkyboxTheme::Countryside);

        REQUIRE(static_cast<int>(lightmap.at({ 1u, 1u })) == 255);
        REQUIRE(static_cast<int>(lightmap.at({ 0u, 1u })) == 192);
        REQUIRE(static_cast<int>(lightmap.at({ 0u, 0u })) == 128);
    }

    SECTION("Correct horizontal light decay")
    {
        auto&& level = prepareLevel(3u, 1u);
        level.mesh.layers[0].tiles = { 3, 0, 0 };
        level.mesh.layers[0].blocks = { 1, 0, 0 };
        level.mesh.layers[1].tiles = { 6, 6, 6 };
        level.mesh.layers[1].blocks = { 1, 1, 1 };

        auto lightmap =
            LightmapBuilder::buildLightmap(level, SkyboxTheme::Countryside);

        REQUIRE(static_cast<int>(lightmap.at({ 1u, 0u })) == 255);
        REQUIRE(static_cast<int>(lightmap.at({ 2u, 0u })) == 224);
    }

    SECTION("Upper light skips bottom walls")
    {
        auto&& level = prepareLevel(4u, 1u);
        level.mesh.layers[0].tiles = { 6, 0, 6, 0 };
        level.mesh.layers[0].blocks = { 1, 0, 1, 0 };
        level.mesh.layers[1].tiles = { 3, 0, 0, 0 };
        level.mesh.layers[1].blocks = { 1, 0, 0, 0 };

        auto lightmap =
            LightmapBuilder::buildLightmap(level, SkyboxTheme::Countryside);

        REQUIRE(static_cast<int>(lightmap.at({ 1u, 0u })) == 255);
        REQUIRE(static_cast<int>(lightmap.at({ 2u, 0u })) == 224);
        REQUIRE(static_cast<int>(lightmap.at({ 3u, 0u })) == 192);
    }

    SECTION("Bottom light stops at bottom wall")
    {
        auto&& level = prepareLevel(4u, 1u);
        level.mesh.layers[0].tiles = { 3, 0, 6, 0 };
        level.mesh.layers[0].blocks = { 1, 0, 1, 0 };
        level.mesh.layers[1].tiles = { 6, 0, 0, 0 };
        level.mesh.layers[1].blocks = { 1, 0, 0, 0 };

        auto lightmap =
            LightmapBuilder::buildLightmap(level, SkyboxTheme::Countryside);

        REQUIRE(static_cast<int>(lightmap.at({ 1u, 0u })) == 255);
        REQUIRE(static_cast<int>(lightmap.at({ 3u, 0u })) == 32);
    }

    SECTION("The darkest place is under the candle")
    {
        auto&& level = prepareLevel(3u, 1u);
        level.mesh.layers[0].tiles = { 0, 0, 0 };
        level.mesh.layers[0].blocks = { 0, 0, 0 };
        level.mesh.layers[1].tiles = { 0, 3, 0 };
        level.mesh.layers[1].blocks = { 0, 1, 0 };

        auto lightmap =
            LightmapBuilder::buildLightmap(level, SkyboxTheme::Countryside);

        REQUIRE(static_cast<int>(lightmap.at({ 0u, 0u })) == 255);
        REQUIRE(static_cast<int>(lightmap.at({ 1u, 0u })) == 32);
        REQUIRE(static_cast<int>(lightmap.at({ 2u, 0u })) == 255);
    }

    SECTION("Bug - full wall does not stop light")
    {
        auto&& level = prepareLevel(3u, 1u);
        level.mesh.layers[0].tiles = { 0, 6, 0 };
        level.mesh.layers[0].blocks = { 0, 1, 0 };
        level.mesh.layers[1].tiles = { 3, 6, 2 }; // sky, wall, ceil
        level.mesh.layers[1].blocks = { 0, 1, 0 };

        auto lightmap =
            LightmapBuilder::buildLightmap(level, SkyboxTheme::Countryside);

        REQUIRE(static_cast<int>(lightmap.at({ 0u, 0u })) == 255u);
        REQUIRE(static_cast<int>(lightmap.at({ 2u, 0u })) == 32u);
    }

    SECTION("Total darkness is 32")
    {
        auto&& level = prepareLevel(3u, 1u);
        level.mesh.layers[0].tiles = { 0, 1, 0 };
        level.mesh.layers[0].blocks = { 0, 0, 0 };
        level.mesh.layers[1].tiles = { 6, 6, 6 }; // sky, wall, ceil
        level.mesh.layers[1].blocks = { 1, 1, 1 };

        auto lightmap =
            LightmapBuilder::buildLightmap(level, SkyboxTheme::Countryside);

        REQUIRE(static_cast<int>(lightmap.at({ 0u, 0u })) == 32);
        REQUIRE(static_cast<int>(lightmap.at({ 1u, 0u })) == 32);
        REQUIRE(static_cast<int>(lightmap.at({ 2u, 0u })) == 32);
    }
}

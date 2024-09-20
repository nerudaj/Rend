#include "Utilities/Utilities.hpp"
#include "TestHelpers/SceneBuilder.hpp"
#include <catch.hpp>
#include <utils/WinConditionStrategy.hpp>

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

    SECTION("Win condition")
    {
        auto options = GameOptions {
            .gameMode = GameMode::SingleFlagCtf,
            .pointlimit = 3,
        };

        auto isPointLimitReached =
            WinConditionStrategy::getWinConditionStrategy(options);

        auto mesh = createDummyMesh();
        auto scene = createDummyScene(mesh);

        SECTION("Returns true if red team won")
        {
            scene.playerStates = std::vector<PlayerState> {
                PlayerState { .inventory = { .score = 1, .team = Team::Red } },
                PlayerState { .inventory = { .score = 1, .team = Team::Blue } },
                PlayerState { .inventory = { .score = 0, .team = Team::Blue } },
                PlayerState { .inventory = { .score = 2, .team = Team::Red } },
            };

            REQUIRE(isPointLimitReached(scene));
        }

        SECTION("Returns true if blue team won")
        {
            scene.playerStates = std::vector<PlayerState> {
                PlayerState { .inventory = { .score = 1, .team = Team::Red } },
                PlayerState { .inventory = { .score = 2, .team = Team::Blue } },
                PlayerState { .inventory = { .score = 2, .team = Team::Blue } },
                PlayerState { .inventory = { .score = 0, .team = Team::Red } },
            };

            REQUIRE(isPointLimitReached(scene));
        }

        SECTION("Returns false if no team won yet")
        {
            scene.playerStates = std::vector<PlayerState> {
                PlayerState { .inventory = { .score = 1, .team = Team::Red } },
                PlayerState { .inventory = { .score = 1, .team = Team::Blue } },
                PlayerState { .inventory = { .score = 0, .team = Team::Blue } },
                PlayerState { .inventory = { .score = 0, .team = Team::Red } },
            };

            REQUIRE_FALSE(isPointLimitReached(scene));
        }
    }
}
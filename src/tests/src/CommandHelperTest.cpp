#include "include/Commands/CommandHelper.hpp"
#include <catch.hpp>

LevelD::Thing createThing(unsigned x, unsigned y)
{
	return LevelD::Thing{
		.id = 0,
		.tag = 0,
		.x = x,
		.y = y,
		.flags = 0,
		.metadata = ""
	};
}

TEST_CASE("[CommandHelper]")
{
	SECTION("getBoundingBox")
	{
		auto getPos = [] (const LevelD::Thing& t)
		{
			return sf::Vector2i(t.x, t.y);
		};

		SECTION("Returns empty optional when no items")
		{
			const auto boundingBox = CommandHelper::getBoundingBox<LevelD::Thing>(
				{},
				sf::Vector2i(1000, 10000),
				getPos);

			REQUIRE(!boundingBox);
		}

		SECTION("Returns valid box if there are items")
		{
			std::vector<LevelD::Thing> things;
			things.push_back(createThing(42, 69));
			things.push_back(createThing(106, 65));
			things.push_back(createThing(54, 72));
			things.push_back(createThing(24, 6969));

			const auto boundingBox = CommandHelper::getBoundingBox<LevelD::Thing>(
				things,
				sf::Vector2i(1000, 10000),
				getPos);

			REQUIRE(boundingBox.has_value());
			REQUIRE(boundingBox->left == 24);
			REQUIRE(boundingBox->top == 65);
			REQUIRE(boundingBox->right == 106);
			REQUIRE(boundingBox->bottom == 6969);
		}
	}
}

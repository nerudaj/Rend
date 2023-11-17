#include <catch.hpp>
#include "include/Commands/CreateDeleteObjectCommand.hpp"
#include "LevelD.hpp"

[[nodiscatd]]
constexpr bool operator==(
	const LevelD::Thing& left,
	const LevelD::Thing& right)
{
	return left.id == right.id
		&& left.x == right.x
		&& left.y == right.y
		&& left.tag == right.tag
		&& left.flags == right.flags
		&& left.metadata == right.metadata;
}

[[nodiscard]]
LevelD::Thing CreateItem(unsigned id, unsigned x, unsigned y)
{
	return LevelD::Thing{
		.id = id,
		.tag = id,
		.x = x,
		.y = y,
		.flags = 0,
		.metadata = "lorem ipsum"
	};
}

TEST_CASE("[Create/DeleteItemCommand]")
{
	std::vector<LevelD::Thing> items;

	for (unsigned i = 0; i < 10; i++)
		items.push_back(CreateItem(i, i * 10, i * 100));

	SECTION("Correctly adds items")
	{
		auto command = CreateItemCommand(
			items,
			CreateItem(69, 24, 42));
		command.exec();

		REQUIRE(items.size() == 11);
		REQUIRE(items[10].id == 69);
	}

	SECTION("Undo->Redo (Create->Delete) works")
	{
		auto command = CreateItemCommand(
			items,
			CreateItem(69, 24, 42));
		command.exec();

		REQUIRE(items.size() == 11);

		auto undoCommand = command.getInverse();
		undoCommand->exec();

		REQUIRE(items.size() == 10);
	}

	SECTION("Correctly removes unordered list of ids")
	{
		auto command = DeleteItemCommand(items, { 9, 3, 5, 1, 4, });
		command.exec();

		REQUIRE(items.size() == 5u);
		REQUIRE(items[0].id == 0);
		REQUIRE(items[1].id == 2);
		REQUIRE(items[2].id == 6);
		REQUIRE(items[3].id == 7);
		REQUIRE(items[4].id == 8);
	}

	SECTION("Undo->Redo (Delete->Create) works")
	{
		auto backup = items;
		auto command = DeleteItemCommand(items, { 9, 3, 5, 1, 4 });
		command.exec();

		REQUIRE(items.size() == 5);
		auto backup2 = items;

		auto undoCommand = command.getInverse();
		undoCommand->exec();

		REQUIRE(items.size() == 10);
		REQUIRE(items == backup);

		// Repeated undo-redo should delete the same items
		command.exec();
		REQUIRE(items.size() == 5);
		REQUIRE(items == backup2);
	}
}

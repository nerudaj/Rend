#include <catch.hpp>
#include "include/Commands/CommandHistory.hpp"

struct TestBlackboard
{
	unsigned execCalled = 0;
	unsigned getInverseCalled = 0;
};

class TestCommand : public UndoableCommandInterface
{
protected:
	TestBlackboard& board;

public:
	TestCommand(TestBlackboard& testBlackboard)
		: board(testBlackboard)
	{}

public:
	void exec() override
	{
		board.execCalled++;
	}

	Box<CommandInterface> getInverse() const override
	{
		board.getInverseCalled++;
		return Box<TestCommand>(board);
	}
};

TEST_CASE("[CommandHistory]")
{
	TestBlackboard board;
	CommandHistory history;

	SECTION("Just adding items appends them")
	{
		history.add(GC<TestCommand>(board));
		REQUIRE(history.test_getCommands().size() == 1u);
		REQUIRE(history.test_getIndex() == 1u);

		history.add(GC<TestCommand>(board));
		REQUIRE(history.test_getCommands().size() == 2u);
		REQUIRE(history.test_getIndex() == 2u);
	}

	SECTION("Add item, undo, then add will overwrite undoed command")
	{
		history.add(GC<TestCommand>(board));
		REQUIRE(history.test_getCommands().size() == 1u);
		REQUIRE(history.test_getIndex() == 1u);

		history.undo();
		REQUIRE(history.test_getCommands().size() == 1u);
		REQUIRE(history.test_getIndex() == 0u);
		REQUIRE(board.getInverseCalled == 1u);
		REQUIRE(board.execCalled == 1u);

		history.add(GC<TestCommand>(board));
		REQUIRE(history.test_getCommands().size() == 1u);
		REQUIRE(history.test_getIndex() == 1u);
	}

	SECTION("Undoing empty history does nothing")
	{
		history.undo();
		REQUIRE(board.getInverseCalled == 0u);
		REQUIRE(board.execCalled == 0u);
		REQUIRE(history.test_getIndex() == 0u);
	}

	SECTION("Add item, undo, redo, add item")
	{
		history.add(GC<TestCommand>(board));

		history.undo();
		REQUIRE(history.test_getCommands().size() == 1u);
		REQUIRE(history.test_getIndex() == 0u);

		history.redo();
		REQUIRE(history.test_getCommands().size() == 1u);
		REQUIRE(history.test_getIndex() == 1u);

		history.add(GC<TestCommand>(board));
		REQUIRE(history.test_getCommands().size() == 2u);
		REQUIRE(history.test_getIndex() == 2u);
	}

	SECTION("Redoing not undoed history does nothing")
	{
		history.add(GC<TestCommand>(board));
		history.redo();
		REQUIRE(board.getInverseCalled == 0u);
		REQUIRE(board.execCalled == 0u);
	}
}
#include "Commands/CommandHistory.hpp"
#include <Memory.hpp>
#include <catch.hpp>

struct [[nodiscard]] TestBlackboard final
{
    unsigned execCalled = 0;
    unsigned getInverseCalled = 0;
};

class [[nodiscard]] TestCommand final : public UndoableCommandInterface
{
protected:
    TestBlackboard& board;

public:
    explicit TestCommand(TestBlackboard& testBlackboard) : board(testBlackboard)
    {
    }

public:
    void exec() override
    {
        board.execCalled++;
    }

    mem::Box<CommandInterface> getInverse() const override
    {
        board.getInverseCalled++;
        return mem::Box<TestCommand>(board);
    }
};

TEST_CASE("[CommandHistory]")
{
    TestBlackboard board;
    CommandHistory history;

    SECTION("Just adding items appends them")
    {
        history.add(mem::Rc<TestCommand>(board));
        REQUIRE(history.test_getCommands().size() == 1u);
        REQUIRE(history.test_getIndex() == 1u);

        history.add(mem::Rc<TestCommand>(board));
        REQUIRE(history.test_getCommands().size() == 2u);
        REQUIRE(history.test_getIndex() == 2u);
    }

    SECTION("Add item, undo, then add will overwrite undoed command")
    {
        history.add(mem::Rc<TestCommand>(board));
        REQUIRE(history.test_getCommands().size() == 1u);
        REQUIRE(history.test_getIndex() == 1u);

        history.undo();
        REQUIRE(history.test_getCommands().size() == 1u);
        REQUIRE(history.test_getIndex() == 0u);
        REQUIRE(board.getInverseCalled == 1u);
        REQUIRE(board.execCalled == 1u);

        history.add(mem::Rc<TestCommand>(board));
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
        history.add(mem::Rc<TestCommand>(board));

        history.undo();
        REQUIRE(history.test_getCommands().size() == 1u);
        REQUIRE(history.test_getIndex() == 0u);

        history.redo();
        REQUIRE(history.test_getCommands().size() == 1u);
        REQUIRE(history.test_getIndex() == 1u);

        history.add(mem::Rc<TestCommand>(board));
        REQUIRE(history.test_getCommands().size() == 2u);
        REQUIRE(history.test_getIndex() == 2u);
    }

    SECTION("Redoing not undoed history does nothing")
    {
        history.add(mem::Rc<TestCommand>(board));
        history.redo();
        REQUIRE(board.getInverseCalled == 0u);
        REQUIRE(board.execCalled == 0u);
    }
}
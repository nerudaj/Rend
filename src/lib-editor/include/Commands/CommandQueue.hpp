#pragma once

#include "Commands/CommandHistory.hpp"
#include "Interfaces/UndoableCommandInterface.hpp"
#include <Memory.hpp>
#include <memory>
#include <queue>

class [[nodiscard]] CommandQueue final
{
protected:
    mem::Rc<CommandHistory> history;
    std::queue<mem::Rc<UndoableCommandInterface>> commands;

public:
    CommandQueue(mem::Rc<CommandHistory> history) : history(history) {}

    CommandQueue(CommandQueue&&) = delete;
    CommandQueue(const CommandQueue&) = delete;

public:
    template<UndoableCommand T, class... Args>
        requires std::constructible_from<T, Args...>
    void push(Args&&... args)
    {
        commands.push(mem::Rc<T>(std::forward<Args>(args)...));
    }

    void processAll();

    [[nodiscard]] bool isEmpty() const noexcept(noexcept(commands.empty()))
    {
        return commands.empty();
    }
};

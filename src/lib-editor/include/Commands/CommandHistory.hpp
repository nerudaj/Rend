#pragma once

#include "Interfaces/UndoableCommandInterface.hpp"
#include <Memory.hpp>
#include <vector>

class [[nodiscard]] CommandHistory final
{
    std::vector<mem::Rc<UndoableCommandInterface>> commands;
    unsigned index = 0;

public:
    void add(const mem::Rc<UndoableCommandInterface>& command);

    void undo();

    void redo();

public:
    [[nodiscard]] constexpr const auto& test_getCommands() const noexcept
    {
        return commands;
    }

    [[nodiscard]] constexpr const auto& test_getIndex() const noexcept
    {
        return index;
    }
};
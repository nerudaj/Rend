#pragma once

#include "Interfaces/CommandInterface.hpp"
#include <Memory.hpp>

class [[nodiscard]] UndoableCommandInterface : public CommandInterface
{
public:
    [[nodiscard]] virtual mem::Box<CommandInterface> getInverse() const = 0;
};

template<class T>
concept UndoableCommand = std::derived_from<T, UndoableCommandInterface>;

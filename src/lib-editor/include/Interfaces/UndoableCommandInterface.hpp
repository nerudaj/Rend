#pragma once

#include "Interfaces/CommandInterface.hpp"

import Memory;

class UndoableCommandInterface : public CommandInterface
{
public:
    [[nodiscard]] virtual mem::Box<CommandInterface> getInverse() const = 0;
};

template<class T>
concept UndoableCommand = std::derived_from<T, UndoableCommandInterface>;

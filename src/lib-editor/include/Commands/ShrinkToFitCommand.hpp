#pragma once

#include "Interfaces/EditorInterface.hpp"
#include "Interfaces/UndoableCommandInterface.hpp"
#include <LevelD.hpp>

class ShrinkToFitCommand final : public UndoableCommandInterface
{
public:
    [[nodiscard]] ShrinkToFitCommand(EditorInterface& editor) noexcept
        : editor(editor)
    {
    }

public:
    void exec() override;

    [[nodiscard]] mem::Box<CommandInterface> getInverse() const override;

protected:
    EditorInterface& editor;
    LevelD levelSnapshot;
};

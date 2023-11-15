#pragma once

#include "Interfaces/EditorInterface.hpp"
#include "Interfaces/UndoableCommandInterface.hpp"
#include <LevelD.hpp>

class ResizeCommand final : public UndoableCommandInterface
{
protected:
    EditorInterface& editor;
    unsigned width;
    unsigned height;
    bool isTranslationDisabled;
    LevelD levelSnapshot;

public:
    [[nodiscard]] ResizeCommand(
        EditorInterface& editor,
        unsigned width,
        unsigned height,
        bool isTranslationDisabled) noexcept
        : editor(editor)
        , width(width)
        , height(height)
        , isTranslationDisabled(isTranslationDisabled)
    {
    }

public:
    void exec() override;

    [[nodiscard]] mem::Box<CommandInterface> getInverse() const override;
};

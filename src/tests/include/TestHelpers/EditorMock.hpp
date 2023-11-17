#pragma once

#include "include/Editor/Editor.hpp"

struct EditorMockState
{
    unsigned drawCallCounter = 0;
    unsigned initCallCounter = 0;
    unsigned handleEventCallCounter = 0;
    unsigned saveToFileCallCounter = 0;
    unsigned loadFromFileCallCounter = 0;
    unsigned switchToolCallCounter = 0;
    unsigned resizeDialogCallCounter = 0;
    unsigned shrinkToFitCallCounter = 0;
};

class EditorMock final : public EditorInterface
{
public:
    EditorMockState* state;

    virtual void draw() override
    {
        state->drawCallCounter++;
    }

    virtual void init(unsigned, unsigned, const std::filesystem::path&) override
    {
        state->initCallCounter++;
    }

    virtual void handleEvent(const sf::Event&, const sf::Vector2i&) override
    {
        state->handleEventCallCounter++;
    }

    virtual LevelD save() const override
    {
        state->saveToFileCallCounter++;
        return {};
    }

    virtual void loadFrom(
        const LevelD&,
        const std::filesystem::path&,
        bool skipInit = false) override
    {
        state->saveToFileCallCounter++;
    }

    virtual void switchTool(EditorState) override
    {
        state->switchToolCallCounter++;
    }

    virtual void resizeDialog() override
    {
        state->resizeDialogCallCounter++;
    }

    virtual void
    resize(unsigned width, unsigned height, bool isTranslationDisabled) override
    {
    }

    virtual void shrinkToFit() override
    {
        state->shrinkToFitCallCounter++;
    }

    EditorMock(EditorMockState* state) : state(state) {}
};
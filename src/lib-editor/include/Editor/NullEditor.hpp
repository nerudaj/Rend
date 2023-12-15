#pragma once

#include "Interfaces/EditorInterface.hpp"

class NullEditor final : public EditorInterface
{
public:
    void draw() override {}

    void init(unsigned, unsigned, const std::filesystem::path&) override {}

    void handleEvent(const sf::Event&, const sf::Vector2i&) override {}

    [[nodiscard]] LevelD save() override
    {
        return {};
    }

    void loadFrom(const LevelD&, const std::filesystem::path&, bool) override {}

    void switchTool(EditorState) override {}

    void resizeDialog() override {}

    void resize(unsigned, unsigned, bool) override {}

    void shrinkToFit() override {}
};

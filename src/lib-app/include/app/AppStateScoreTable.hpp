#pragma once

import Memory;

#include "GameSettings.hpp"
#include "GuiState.hpp"
#include "utils/DependencyContainer.hpp"
#include <DGM/classes/AppState.hpp>
#include <DGM/classes/Traits.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

class [[nodiscard]] AppStateScoreTable final
    : public dgm::AppState
    , public GuiState
{
public:
    AppStateScoreTable(
        dgm::App& app,
        mem::Rc<DependencyContainer> dic,
        const GameOptions& gameSettings,
        dgm::UniversalReference<std::vector<int>> auto&& scores)
        : dgm::AppState(
            app, dgm::AppStateConfig { .clearColor = sf::Color::White })
        , GuiState(dic)
        , gameSettings(gameSettings)
        , scores(std::forward<decltype(scores)>(scores))
    {
        buildLayout();
        dic->jukebox->playInterludeSong();
    }

public:
    void input() override;

    void update() override {}

    void draw() override
    {
        dic->gui->draw();
    }

private:
    void buildLayoutImpl() override;

private:
    GameOptions gameSettings;
    std::vector<int> scores;
    float transitionTimeout = 4.f; // seconds
};
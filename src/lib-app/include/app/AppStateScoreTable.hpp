#pragma once

#include "GameSettings.hpp"
#include "utils/DependencyContainer.hpp"
#include <DGM/classes/AppState.hpp>
#include <DGM/classes/Traits.hpp>
#include <Memory.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

class [[nodiscard]] AppStateScoreTable final : public dgm::AppState
{
public:
    AppStateScoreTable(
        dgm::App& app,
        mem::Rc<DependencyContainer> dic,
        const GameOptions& gameSettings,
        dgm::UniversalReference<std::vector<int>> auto&& scores)
        : dgm::AppState(
            app, dgm::AppStateConfig { .clearColor = sf::Color::White })
        , dic(dic)
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
    void buildLayout();

private:
    mem::Rc<DependencyContainer> dic;
    GameOptions gameSettings;
    std::vector<int> scores;
    float transitionTimeout = 4.f; // seconds
};
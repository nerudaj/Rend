#pragma once

import Memory;
import Options;
import Audio;

#include "GuiState.hpp"
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
        mem::Rc<tgui::Gui> gui,
        mem::Rc<AudioPlayer> audioPlayer,
        mem::Rc<Jukebox> jukebox,
        const GameOptions& gameSettings,
        dgm::UniversalReference<std::vector<int>> auto&& scores)
        : dgm::AppState(app)
        , GuiState(gui, audioPlayer)
        , gui(gui)
        , jukebox(jukebox)
        , gameSettings(gameSettings)
        , scores(std::forward<decltype(scores)>(scores))
    {
        buildLayout();
        jukebox->playInterludeSong();
    }

public:
    void input() override;

    void update() override {}

    void draw() override
    {
        gui->draw();
    }

    [[nodiscard]] bool isTransparent() const noexcept override
    {
        return false;
    }

    [[nodiscard]] sf::Color getClearColor() const override
    {
        return sf::Color::White;
    }

private:
    void buildLayoutImpl() override;

private:
    mem::Rc<tgui::Gui> gui;
    mem::Rc<Jukebox> jukebox;
    GameOptions gameSettings;
    std::vector<int> scores;
    float transitionTimeout = 4.f; // seconds
};
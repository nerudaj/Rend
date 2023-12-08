#pragma once

import Memory;
import Options;
import Audio;

#include "GuiState.hpp"
#include <DGM/classes/AppState.hpp>
#include <DGM/classes/Traits.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

class [[nodiscard]] AppStateWinnerAnnounced final
    : public dgm::AppState
    , public GuiState
{
public:
    AppStateWinnerAnnounced(
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
    }

public:
    void input() override;

    void update() override;

    void draw() override
    {
        gui->draw();
    }

    [[nodiscard]] bool isTransparent() const noexcept override
    {
        return true;
    }

private:
    void buildLayoutImpl() override;

private:
    mem::Rc<tgui::Gui> gui;
    mem::Rc<Jukebox> jukebox;
    GameOptions gameSettings;
    std::vector<int> scores;
    float transitionTimeout = 3.f; // seconds
};
#pragma once

import Memory;
import Options;
import Audio;
import Input;

#include "GuiState.hpp"
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <dgm/classes/AppState.hpp>
#include <dgm/classes/ResourceManager.hpp>

class [[nodiscard]] AppStateGameSetup final
    : public dgm::AppState
    , public GuiState
{
public:
    AppStateGameSetup(
        dgm::App& app,
        mem::Rc<const dgm::ResourceManager> resmgr,
        mem::Rc<tgui::Gui> gui,
        mem::Rc<AppOptions> settings,
        mem::Rc<AudioPlayer> audioPlayer,
        mem::Rc<Jukebox> jukebox,
        mem::Rc<PhysicalController> controller) noexcept;

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

    void restoreFocus() override
    {
        GuiState::restoreFocus(app.window.getWindowContext());
    }

private:
    void buildLayoutImpl() override;

    void startGame();

    [[nodiscard]] std::vector<PlayerOptions> createPlayerSettings() const;

private:
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<tgui::Gui> gui;
    mem::Rc<AppOptions> settings;
    mem::Rc<AudioPlayer> audioPlayer;
    mem::Rc<Jukebox> jukebox;
    mem::Rc<PhysicalController> controller;
    int fraglimit;
    unsigned playerCount;
    std::string mapname;
    std::vector<std::string> mapnames;
};

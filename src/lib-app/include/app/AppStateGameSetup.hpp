#pragma once

import Memory;

#include "GuiState.hpp"
#include <Settings.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <audio/AudioPlayer.hpp>
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
        mem::Rc<Settings> settings,
        mem::Rc<AudioPlayer> audioPlayer) noexcept;

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

    [[nodiscard]] std::vector<PlayerSettings> createPlayerSettings() const;

private:
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<tgui::Gui> gui;
    mem::Rc<Settings> settings;
    mem::Rc<AudioPlayer> audioPlayer;
    unsigned fraglimit;
    unsigned playerCount;
    std::string mapname;
};

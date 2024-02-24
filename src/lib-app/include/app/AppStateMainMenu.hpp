#pragma once

import Memory;
import Options;
import Audio;
import Input;

#include "GuiState.hpp"
#include <vector>

class [[nodiscard]] AppStateMainMenu final
    : public dgm::AppState
    , public GuiState
{
public:
    AppStateMainMenu(
        dgm::App& app,
        mem::Rc<const dgm::ResourceManager> resmgr,
        mem::Rc<tgui::Gui> gui,
        mem::Rc<AudioPlayer> audioPlayer,
        mem::Rc<Jukebox> jukebox,
        mem::Rc<AppOptions> settings,
        mem::Rc<PhysicalController> controller)
        : dgm::AppState(
            app, dgm::AppStateConfig { .clearColor = sf::Color::White })
        , GuiState(gui, audioPlayer)
        , resmgr(resmgr)
        , jukebox(jukebox)
        , settings(settings)
        , controller(controller)
    {
        buildLayout();
        jukebox->playTitleSong();
    }

public:
    void input() override;

    void update() override {}

    void draw() override
    {
        gui->draw();
    }

private:
    void goToGameSetup();

    void buildLayoutImpl() override;

    void restoreFocusImpl(const std::string&) override
    {
        app.window.getWindowContext().setTitle("Rend");
        GuiState::restoreFocus(app.window.getWindowContext());
        jukebox->playTitleSong();
    }

private:
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<Jukebox> jukebox;
    mem::Rc<AppOptions> settings;
    mem::Rc<PhysicalController> controller;
};

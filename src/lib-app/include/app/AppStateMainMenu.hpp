#pragma once

import Memory;
import Options;
import Audio;
import Input;

#include "GuiState.hpp"
#include <vector>

class AppStateMainMenu final
    : public dgm::AppState
    , public GuiState
{
public:
    [[nodiscard]] AppStateMainMenu(
        dgm::App& app,
        mem::Rc<const dgm::ResourceManager> resmgr,
        mem::Rc<tgui::Gui> gui,
        mem::Rc<AudioPlayer> audioPlayer,
        mem::Rc<Jukebox> jukebox,
        mem::Rc<AppOptions> settings,
        mem::Rc<PhysicalController> controller)
        : dgm::AppState(app)
        , GuiState(gui, audioPlayer)
        , resmgr(resmgr)
        , jukebox(jukebox)
        , settings(settings)
        , controller(controller)
    {
        buildLayout();
        jukebox->playTitleSong();
    }

private:
    void buildLayoutImpl() override;

public:
    virtual void input() override;

    virtual void update() override {}

    virtual void draw() override
    {
        gui->draw();
    }

    virtual [[nodiscard]] bool isTransparent() const noexcept override
    {
        return false;
    }

    virtual [[nodiscard]] sf::Color getClearColor() const override
    {
        return sf::Color::White;
    }

    virtual void restoreFocus() override
    {
        app.window.getWindowContext().setTitle("Rend");
        GuiState::restoreFocus(app.window.getWindowContext());
        jukebox->playTitleSong();
    }

private:
    void goToGameSetup();

private:
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<Jukebox> jukebox;
    mem::Rc<AppOptions> settings;
    mem::Rc<PhysicalController> controller;
};

#pragma once

#include "GuiState.hpp"

import Options;
import Audio;
import FormBuilder;
import Input;

class [[nodiscard]] AppStateMenuOptions final
    : public dgm::AppState
    , public GuiState
{
public:
    AppStateMenuOptions(
        dgm::App& app,
        mem::Rc<tgui::Gui> gui,
        mem::Rc<const dgm::ResourceManager> resmgr,
        mem::Rc<AudioPlayer> audioPlayer,
        mem::Rc<Jukebox> jukebox,
        mem::Rc<AppOptions> settings,
        mem::Rc<PhysicalController> controller,
        bool enteredFromGame = false)
        : dgm::AppState(
            app, dgm::AppStateConfig { .clearColor = sf::Color::White })
        , GuiState(gui, audioPlayer)
        , resmgr(resmgr)
        , jukebox(jukebox)
        , settings(settings)
        , controller(controller)
        , enteredFromGame(enteredFromGame)
    {
        buildLayout();
    }

public:
    void input() override;

    void update() override {}

    void draw() override
    {
        gui->draw();
    }

private:
    void buildLayoutImpl() override;
    void buildDisplayOptionsLayout(FormBuilder& builder);
    void buildAudioOptionsLayout(FormBuilder& builder);
    void buildInputOptionsLayout(FormBuilder& builder);

    void restoreFocusImpl(const std::string& = "") override
    {
        GuiState::restoreFocus(app.window.getWindowContext());
    }

private:
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<Jukebox> jukebox;
    mem::Rc<AppOptions> settings;
    mem::Rc<PhysicalController> controller;
    bool enteredFromGame;
};
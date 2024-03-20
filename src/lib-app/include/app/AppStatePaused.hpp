#pragma once

#include "GuiState.hpp"

import Options;
import Input;

class [[nodiscard]] AppStatePaused final
    : public dgm::AppState
    , public GuiState
{
public:
    AppStatePaused(
        dgm::App& app,
        mem::Rc<tgui::Gui> gui,
        mem::Rc<const dgm::ResourceManager> resmgr,
        mem::Rc<AudioPlayer> audioPlayer,
        mem::Rc<Jukebox> jukebox,
        mem::Rc<PhysicalController> controller,
        mem::Rc<AppOptions> settings)
        : dgm::AppState(
            app,
            dgm::AppStateConfig { .shouldUpdateUnderlyingState = true,
                                  .shouldDrawUnderlyingState = true })
        , GuiState(gui, audioPlayer)
        , resmgr(resmgr)
        , jukebox(jukebox)
        , controller(controller)
        , settings(settings)
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

    void restoreFocusImpl(const std::string& message) override
    {
        if (PopIfPause::canDeserializeFrom(message)) app.popState();
        GuiState::restoreFocus(app.window.getWindowContext());
    }

private:
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<Jukebox> jukebox;
    mem::Rc<PhysicalController> controller;
    mem::Rc<AppOptions> settings;
};
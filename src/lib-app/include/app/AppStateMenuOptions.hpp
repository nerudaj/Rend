#pragma once

#include "GuiState.hpp"

import Options;
import Audio;
import FormBuilder;
import Input;

class AppStateMenuOptions final
    : public dgm::AppState
    , public GuiState
{
public:
    [[nodiscard]] AppStateMenuOptions(
        dgm::App& app,
        mem::Rc<tgui::Gui> gui,
        mem::Rc<AudioPlayer> audioPlayer,
        mem::Rc<Jukebox> jukebox,
        mem::Rc<AppOptions> settings,
        mem::Rc<PhysicalController> controller)
        : dgm::AppState(app)
        , GuiState(gui, audioPlayer)
        , jukebox(jukebox)
        , settings(settings)
        , controller(controller)
    {
        buildLayout();
    }

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
        GuiState::restoreFocus(app.window.getWindowContext());
    }

private:
    void buildLayoutImpl() override;
    void buildDisplayOptionsLayout(FormBuilder& builder);
    void buildAudioOptionsLayout(FormBuilder& builder);
    void buildInputOptionsLayout(FormBuilder& builder);

private:
    mem::Rc<Jukebox> jukebox;
    mem::Rc<AppOptions> settings;
    mem::Rc<PhysicalController> controller;
};
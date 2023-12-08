#pragma once

#include "GuiState.hpp"

import Options;
import Audio;

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
        mem::Rc<AppOptions> settings)
        : dgm::AppState(app)
        , GuiState(gui, audioPlayer)
        , jukebox(jukebox)
        , settings(settings)
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
    void buildDisplayOptionsLayout(GuiOptionsBuilder2& builder);
    void buildAudioOptionsLayout(GuiOptionsBuilder2& builder);
    void buildInputOptionsLayout(GuiOptionsBuilder2& builder);

private:
    mem::Rc<Jukebox> jukebox;
    mem::Rc<AppOptions> settings;
};
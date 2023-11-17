#pragma once

import Memory;

#include "GameSettings.hpp"
#include "GuiState.hpp"
#include "PlayerSettings.hpp"
#include "Settings.hpp"
#include <vector>

class AppStateMainMenu final
    : public dgm::AppState
    , public GuiState
{
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
    }

    [[nodiscard]] AppStateMainMenu(
        dgm::App& app,
        mem::Rc<const dgm::ResourceManager> resmgr,
        mem::Rc<tgui::Gui> gui,
        mem::Rc<AudioPlayer> audioPlayer,
        mem::Rc<Settings> settings)
        : dgm::AppState(app)
        , GuiState(gui, audioPlayer)
        , settings(settings)
        , resmgr(resmgr)
    {
        buildLayout();
    }

private:
    void goToGameSetup();

private:
    mem::Rc<Settings> settings;
    mem::Rc<const dgm::ResourceManager> resmgr;
};

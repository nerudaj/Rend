#pragma once

#include "GuiState.hpp"
#include "Settings.hpp"

class AppStatePaused final
    : public dgm::AppState
    , public GuiState
{
public:
    AppStatePaused(
        dgm::App& app,
        mem::Rc<tgui::Gui> gui,
        mem::Rc<AudioPlayer> audioPlayer,
        mem::Rc<Settings> settings)
        : dgm::AppState(app), GuiState(gui, audioPlayer), settings(settings)
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
        return true;
    }

    virtual void restoreFocus() override
    {
        GuiState::restoreFocus(app.window.getWindowContext());
    }

private:
    void buildLayoutImpl() override;

private:
    mem::Rc<Settings> settings;
};
#pragma once

#include "GuiState.hpp"

import Options;

class [[nodiscard]] AppStatePaused final
    : public dgm::AppState
    , public GuiState
{
public:
    AppStatePaused(
        dgm::App& app,
        mem::Rc<tgui::Gui> gui,
        mem::Rc<AudioPlayer> audioPlayer,
        mem::Rc<AppOptions> settings)
        : dgm::AppState(app), GuiState(gui, audioPlayer), settings(settings)
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

    [[nodiscard]] bool isTransparent() const noexcept override
    {
        return true;
    }

    void restoreFocus() override
    {
        GuiState::restoreFocus(app.window.getWindowContext());
    }

private:
    void buildLayoutImpl() override;

private:
    mem::Rc<AppOptions> settings;
};
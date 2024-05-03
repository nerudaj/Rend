#pragma once

import Memory;
import Options;
import Audio;
import Input;

#include "GuiState.hpp"
#include "utils/DependencyContainer.hpp"
#include <vector>

class [[nodiscard]] AppStateMainMenu final
    : public dgm::AppState
    , public GuiState
{
public:
    AppStateMainMenu(dgm::App& app, mem::Rc<DependencyContainer> dic)
        : dgm::AppState(
            app, dgm::AppStateConfig { .clearColor = sf::Color::White })
        , GuiState(dic)
    {
        buildLayout();
        dic->jukebox->playTitleSong();
    }

public:
    void input() override;

    void update() override {}

    void draw() override
    {
        dic->gui->draw();
    }

private:
    void goToGameSetup();

    void buildLayoutImpl() override;

    void restoreFocusImpl(const std::string&) override
    {
        app.window.getWindowContext().setTitle("Rend");
        GuiState::restoreFocus(app.window.getWindowContext());
        dic->jukebox->playTitleSong();
    }
};

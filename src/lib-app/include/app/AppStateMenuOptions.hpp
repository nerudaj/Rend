#pragma once

#include "GuiState.hpp"

import Options;
import Audio;
import FormBuilder;
import Input;
import DependencyContainer;

class [[nodiscard]] AppStateMenuOptions final
    : public dgm::AppState
    , public GuiState
{
public:
    AppStateMenuOptions(
        dgm::App& app,
        mem::Rc<DependencyContainer> dic,
        bool enteredFromGame = false)
        : dgm::AppState(
            app,
            dgm::AppStateConfig {
                .clearColor = sf::Color::White,
                .shouldUpdateUnderlyingState = enteredFromGame,
            })
        , GuiState(dic)
        , enteredFromGame(enteredFromGame)
    {
        buildLayout();
    }

public:
    void input() override;

    void update() override {}

    void draw() override
    {
        dic->gui->draw();
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
    bool enteredFromGame;
};
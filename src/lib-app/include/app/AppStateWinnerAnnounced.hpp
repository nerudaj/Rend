#pragma once

import Memory;
import Options;
import Audio;
import Input;

#include "GuiState.hpp"
#include "utils/DependencyContainer.hpp"
#include <DGM/classes/AppState.hpp>
#include <DGM/classes/Traits.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

class [[nodiscard]] AppStateWinnerAnnounced final
    : public dgm::AppState
    , public GuiState
{
public:
    AppStateWinnerAnnounced(
        dgm::App& app,
        mem::Rc<DependencyContainer> dic,
        const GameOptions& gameSettings,
        dgm::UniversalReference<std::vector<int>> auto&& scores)
        // TODO: should update underlying state as well
        : dgm::AppState(
            app, dgm::AppStateConfig { .shouldDrawUnderlyingState = true })
        , GuiState(dic)
        , gameSettings(gameSettings)
        , scores(std::forward<decltype(scores)>(scores))
    {
        buildLayout();
    }

public:
    void input() override;

    void update() override;

    void draw() override
    {
        dic->gui->draw();
    }

private:
    void buildLayoutImpl() override;

    void restoreFocusImpl(const std::string& message) override
    {
        handleRestoreMessage(app, message);
    }

private:
    GameOptions gameSettings;
    std::vector<int> scores;
    float transitionTimeout = 3.f; // seconds
};
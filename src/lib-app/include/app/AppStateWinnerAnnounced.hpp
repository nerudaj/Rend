#pragma once

#include "GameSettings.hpp"
#include "utils/DependencyContainer.hpp"
#include <Client.hpp>
#include <DGM/classes/AppState.hpp>
#include <DGM/classes/Traits.hpp>
#include <Literals.hpp>
#include <Memory.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

class [[nodiscard]] AppStateWinnerAnnounced final : public dgm::AppState
{
public:
    AppStateWinnerAnnounced(
        dgm::App& app,
        mem::Rc<DependencyContainer> dic,
        mem::Rc<Client> client,
        const GameOptions& gameSettings,
        dgm::UniversalReference<std::vector<int>> auto&& scores)
        : dgm::AppState(
            app, dgm::AppStateConfig { .shouldDrawUnderlyingState = true })
        , dic(dic)
        , client(client)
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
    void buildLayout();

    void restoreFocusImpl(const std::string& message) override;

private:
    mem::Rc<DependencyContainer> dic;
    mem::Rc<Client> client;
    GameOptions gameSettings;
    std::vector<int> scores;
    float transitionTimeout = 3_seconds;
};
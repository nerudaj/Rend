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
        const Scene& scene)
        : dgm::AppState(
            app, dgm::AppStateConfig { .shouldDrawUnderlyingState = true })
        , dic(dic)
        , client(client)
        , gameSettings(gameSettings)
        , scene(scene)
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

    [[nodiscard]] std::string getSoloWinnerString() const;

    [[nodiscard]] std::string getTeamWinnerString() const;

    [[nodiscard]] PlayerStateIndexType getWinnerIndex() const;

private:
    mem::Rc<DependencyContainer> dic;
    mem::Rc<Client> client;
    GameOptions gameSettings;
    const Scene& scene;
    float transitionTimeout = 3_seconds;
};
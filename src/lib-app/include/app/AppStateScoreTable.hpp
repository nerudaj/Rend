#pragma once

#include "GameSettings.hpp"
#include "utils/DependencyContainer.hpp"
#include <Client.hpp>
#include <DGM/classes/AppState.hpp>
#include <DGM/classes/Traits.hpp>
#include <Memory.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

class [[nodiscard]] AppStateScoreTable final : public dgm::AppState
{
public:
    AppStateScoreTable(
        dgm::App& app,
        mem::Rc<DependencyContainer> dic,
        mem::Rc<Client> client,
        const GameOptions& gameSettings,
        dgm::UniversalReference<std::vector<int>> auto&& scores)
        : dgm::AppState(
            app, dgm::AppStateConfig { .clearColor = sf::Color::White })
        , dic(dic)
        , client(client)
        , gameSettings(gameSettings)
        , clientData(scores.size())
        , scores(std::forward<decltype(scores)>(scores))
    {
        buildLayout();
        dic->jukebox->playInterludeSong();
        dic->logger->ifError(0, "sendHeartbeat", client->sendHeartbeat());
    }

public:
    void input() override;

    void update() override {}

    void draw() override
    {
        dic->gui->draw();
    }

private:
    void buildLayout();

    [[nodiscard]] tgui::Panel::Ptr buildTable() const;

    void handleNetworkUpdate(const ServerUpdateData& update);

private:
    mem::Rc<DependencyContainer> dic;
    mem::Rc<Client> client;
    GameOptions gameSettings;
    std::vector<ClientData> clientData;
    std::vector<int> scores;
};
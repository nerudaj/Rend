#pragma once

#include "utils/DependencyContainer.hpp"
#include <Client.hpp>
#include <DGM/dgm.hpp>
#include <SFML/Network.hpp>

class [[nodiscard]] AppStatePeerLobby final : public dgm::AppState
{
public:
    AppStatePeerLobby(
        dgm::App& app,
        mem::Rc<DependencyContainer> dic,
        const sf::IpAddress& hostAddress);

public:
    void input() override;

    void update() override {}

    void draw() override
    {
        dic->gui->draw();
    }

private:
    void buildLayout();

    void restoreFocusImpl(const std::string& message) override;

    void handleLobbyUpdate(const ServerUpdateData& update);

private:
    mem::Rc<DependencyContainer> dic;
    mem::Rc<Client> client;
    ClientData myPeerData;
    std::vector<ClientData> connectedPeers;
    LobbySettings lobbySettings;
};
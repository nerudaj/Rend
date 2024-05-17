#pragma once

#include "GameSettings.hpp"
#include "utils/DependencyContainer.hpp"
#include <Client.hpp>
#include <DGM/dgm.hpp>
#include <Memory.hpp>

class [[nodiscard]] AppStateLobbyBase : public dgm::AppState
{
public:
    AppStateLobbyBase(
        dgm::App& app,
        mem::Rc<DependencyContainer> dic,
        mem::Rc<Client> client);

    virtual ~AppStateLobbyBase() = default;

protected:
    void handleNetworkUpdate(const ServerUpdateData& update);

    void startGame();

    std::vector<PlayerOptions> createPlayerSettings();

    void buildLayout();

    void buildLayoutGameSetup(tgui::Panel::Ptr target)
    {
        buildLayoutGameSetupImpl(target);
    }

    void buildLayoutPlayerSetup(tgui::Panel::Ptr target);

    void buildLayoutPlayerTable(tgui::Panel::Ptr target);

    void handleTabSelected(const tgui::String& tabname);

protected:
    virtual void buildLayoutGameSetupImpl(tgui::Panel::Ptr target) = 0;

protected:
    mem::Rc<DependencyContainer> dic;
    mem::Rc<Client> client;
    ClientData myPeerData;
    std::vector<ClientData> connectedPeers;
    LobbySettings lobbySettings;
};
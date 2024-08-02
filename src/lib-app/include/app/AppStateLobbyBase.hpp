#pragma once

#include "GameSettings.hpp"
#include "utils/DependencyContainer.hpp"
#include <Client.hpp>
#include <DGM/dgm.hpp>
#include <Memory.hpp>

struct LobbyBaseConfig
{
    bool isHost = false;
};

class [[nodiscard]] AppStateLobbyBase : public dgm::AppState
{
public:
    AppStateLobbyBase(
        dgm::App& app,
        mem::Rc<DependencyContainer> dic,
        mem::Rc<Client> client,
        LobbyBaseConfig config = {});

    virtual ~AppStateLobbyBase() = default;

protected:
    void handleNetworkUpdate(const ServerUpdateData& update);

    void handleMapDownload(const MapDownloadResponse& data);

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

    void selectTab(const tgui::String& tabname);

    void checkMapAvailability();

protected:
    virtual void buildLayoutGameSetupImpl(tgui::Panel::Ptr target) = 0;

protected:
    enum class [[nodiscard]] CurrentTab
    {
        None,
        GameSetup,
        PlayerSetup,
        PlayerTable
    };

    mem::Rc<DependencyContainer> dic;
    mem::Rc<Client> client;
    ClientData myPeerData;
    std::vector<ClientData> connectedPeers;
    LobbySettings lobbySettings;
    CurrentTab currentTab = CurrentTab::None;
    LobbyBaseConfig config;
};
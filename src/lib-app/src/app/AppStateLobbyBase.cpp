#include "app/AppStateLobbyBase.hpp"
#include "app/AppStateMapRotationWrapper.hpp"
#include "app/AppStateMenuOptions.hpp"
#include <Configs/Strings.hpp>
#include <GuiBuilder.hpp>
#include <ranges>

AppStateLobbyBase::AppStateLobbyBase(
    dgm::App& app, mem::Rc<DependencyContainer> dic, mem::Rc<Client> client)
    : dgm::AppState(app)
    , dic(dic)
    , client(client)
    , myPeerData(ClientData { .name = dic->settings->player.name,
                              .hasAutoswapOnPickup =
                                  dic->settings->player.autoswapOnPickup })
    , lobbySettings(LobbySettings {
          .fraglimit = static_cast<int>(dic->settings->cmdSettings.fraglimit),
          .maxNpcs = dic->settings->cmdSettings.maxNpcs })
{
    dic->logger->log(client->sendPeerSettingsUpdate(myPeerData));
}

void AppStateLobbyBase::handleNetworkUpdate(const ServerUpdateData& update)
{
    connectedPeers = update.clients;
    if (!update.lobbySettings.isEmpty())
    {
        lobbySettings = update.lobbySettings;
    }

    if (update.state == ServerState::MapLoading)
    {
        startGame();
    }
    else if (currentTab == CurrentTab::None)
    {
        buildLayout();
    }
    else if (currentTab == CurrentTab::PlayerTable)
    {
        handleTabSelected(Strings::AppState::PeerLobby::PLAYERS);
    }
}

void AppStateLobbyBase::startGame()
{
    auto&& maplist =
        lobbySettings.mapSettings
        | std::views::filter([](const MapSettings& ms) { return ms.enabled; })
        | std::views::transform([](const MapSettings& ms) { return ms.name; })
        | std::ranges::to<std::vector>();

    if (maplist.empty()) throw std::runtime_error("No map selected!");

    app.pushState<AppStateMapRotationWrapper>(
        dic,
        client,
        GameOptions { .players = createPlayerSettings(),
                      .fraglimit =
                          static_cast<unsigned>(lobbySettings.fraglimit) },
        lobbySettings.packname,
        maplist);
}

std::vector<PlayerOptions> AppStateLobbyBase::createPlayerSettings()
{
    auto&& playerOptions = std::vector<PlayerOptions>();
    for (auto&& [idx, connectedPeer] : std::views::enumerate(connectedPeers))
    {
        playerOptions.push_back(PlayerOptions {
            .kind = idx == client->getMyIndex() ? PlayerKind::LocalHuman
                                                : PlayerKind::RemoteHuman,
            .bindCamera = idx == client->getMyIndex(),
            .name = connectedPeer.name,
            .autoswapOnPickup = connectedPeer.hasAutoswapOnPickup });
    }

    const auto&& NPC_NAMES = std::vector<std::string> {
        "phobos",
        "spartan",
        "deimos",
    };

    for (auto&& idx : std::views::iota(size_t { 0 }, lobbySettings.maxNpcs))
    {
        playerOptions.push_back(PlayerOptions {
            .kind = PlayerKind::LocalNpc,
            .bindCamera = false,
            .name = NPC_NAMES.at(idx),
        });
    }

    return playerOptions;
}

void AppStateLobbyBase::buildLayout()
{
    auto&& tabs = WidgetBuilder::createTabbedContent(
        { Strings::AppState::PeerLobby::GAME_SETUP,
          Strings::AppState::PeerLobby::PLAYER_SETUP,
          Strings::AppState::PeerLobby::PLAYERS },
        "IdLobbyTabPanel",
        [&](const tgui::String& tabname) { handleTabSelected(tabname); });

    dic->gui->rebuildWith(
        LayoutBuilder::withBackgroundImage(
            dic->resmgr->get<sf::Texture>("menu_setup.png").value().get())
            .withTitle(Strings::AppState::PeerLobby::TITLE, HeadingLevel::H1)
            .withContent(tabs)
            .withBackButton(WidgetBuilder::createButton(
                Strings::AppState::MainMenu::BACK, [&] { app.popState(); }))
            .withSubmitButton(WidgetBuilder::createButton(
                Strings::AppState::PeerLobby::READY,
                [&] { client->sendPeerReadySignal(); }))
            .build());

    handleTabSelected(Strings::AppState::PeerLobby::GAME_SETUP);
}

void AppStateLobbyBase::buildLayoutPlayerSetup(tgui::Panel::Ptr target)
{
    auto&& formBuilder = FormBuilder();
    AppStateMenuOptions::buildPlayerOptionsLayout(
        formBuilder,
        dic,
        false,
        [&]
        {
            myPeerData.name = dic->settings->player.name;
            myPeerData.hasAutoswapOnPickup =
                dic->settings->player.autoswapOnPickup;
            client->sendPeerSettingsUpdate(myPeerData);
        });
    target->add(formBuilder.build(PANEL_BACKGROUND_COLOR));
}

void AppStateLobbyBase::buildLayoutPlayerTable(tgui::Panel::Ptr target)
{
    auto&& table =
        TableBuilder().withHeading({ Strings::AppState::PeerLobby::PNAME,
                                     Strings::AppState::PeerLobby::IS_READY });
    for (auto&& peer : connectedPeers)
        table.addRow({ peer.name,
                       peer.state == ClientState::ConnectedAndReady
                           ? Strings::AppState::PeerLobby::READY
                           : Strings::AppState::PeerLobby::NOT_READY });

    target->add(table.build());
}

void AppStateLobbyBase::handleTabSelected(const tgui::String& tabname)
{
    auto&& panel = dic->gui->get<tgui::Panel>("IdLobbyTabPanel");
    panel->removeAllWidgets();

    if (tabname == Strings::AppState::PeerLobby::GAME_SETUP)
    {
        currentTab = CurrentTab::GameSetup;
        buildLayoutGameSetup(panel);
    }
    else if (tabname == Strings::AppState::PeerLobby::PLAYER_SETUP)
    {
        currentTab = CurrentTab::PlayerSetup;
        buildLayoutPlayerSetup(panel);
    }
    else if (tabname == Strings::AppState::PeerLobby::PLAYERS)
    {
        currentTab = CurrentTab::PlayerTable;
        buildLayoutPlayerTable(panel);
    }
}

#include "app/AppStateLobbyBase.hpp"
#include "app/AppStateMapRotationWrapper.hpp"
#include "app/AppStateMenuOptions.hpp"
#include "utils/ClientStateToString.hpp"
#include "utils/InputHandler.hpp"
#include <Configs/Strings.hpp>
#include <GuiBuilder.hpp>
#include <base64.hpp>
#include <ranges>

constexpr const char* TABS_ID = "IdLobbyBaseTabs";

AppStateLobbyBase::AppStateLobbyBase(
    dgm::App& app,
    mem::Rc<DependencyContainer> dic,
    mem::Rc<Client> client,
    LobbyBaseConfig config)
    : dgm::AppState(app)
    , dic(dic)
    , client(client)
    , myPeerData(ClientData { .name = dic->settings->player.name,
                              .hasAutoswapOnPickup =
                                  dic->settings->player.autoswapOnPickup })
    , lobbySettings(LobbySettings {
          .fraglimit = static_cast<int>(dic->settings->cmdSettings.fraglimit),
          .maxNpcs = dic->settings->cmdSettings.maxNpcs })
    , config(config)
{
    dic->logger->log(client->sendPeerSettingsUpdate(myPeerData));
}

void AppStateLobbyBase::handleNetworkUpdate(const ServerUpdateData& update)
{
    connectedPeers = update.clients;
    if (!update.lobbySettings.isEmpty())
    {
        lobbySettings = update.lobbySettings;
        checkMapAvailability();
        // TODO: check any unsupported maps
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

void AppStateLobbyBase::handleMapDownload(const MapDownloadResponse& data)
{
    auto decoded = base64::from_base64(data.base64encodedMap);
    auto mapPackDir =
        dic->settings->cmdSettings.resourcesDir / "levels" / data.mapPackName;
    std::filesystem::create_directory(mapPackDir);
    auto save = std::ofstream(mapPackDir / data.mapName);
    save << decoded;
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
        "[bot] phobos",
        "[bot] spartan",
        "[bot] deimos",
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
    const auto tabLabels =
        config.isHost ? std::vector<
            std::string> { Strings::AppState::PeerLobby::GAME_SETUP,
                           Strings::AppState::PeerLobby::PLAYER_SETUP,
                           Strings::AppState::PeerLobby::PLAYERS }
                      : std::vector<std::string> {
                            Strings::AppState::PeerLobby::PLAYER_SETUP,
                            Strings::AppState::PeerLobby::PLAYERS
                        };
    auto&& tabs = WidgetBuilder::createTabbedContent(
        tabLabels,
        "IdLobbyTabPanel",
        [&](const tgui::String& tabname) { handleTabSelected(tabname); },
        TABS_ID);

    dic->gui->rebuildWith(
        LayoutBuilder::withBackgroundImage(
            dic->resmgr->get<sf::Texture>("menu_setup.png").value().get())
            .withTitle(Strings::AppState::PeerLobby::TITLE, HeadingLevel::H1)
            .withContent(tabs)
            .withBackButton(WidgetBuilder::createButton(
                Strings::AppState::MainMenu::BACK, [&] { app.popState(); }))
            .withSubmitButton(WidgetBuilder::createButton(
                Strings::AppState::PeerLobby::READY,
                [&]
                {
                    selectTab(Strings::AppState::PeerLobby::PLAYERS);
                    InputHandler::handleSendReady(app, *dic, client);
                }))
            .build());

    handleTabSelected(
        config.isHost ? Strings::AppState::PeerLobby::GAME_SETUP
                      : Strings::AppState::PeerLobby::PLAYER_SETUP);
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
    auto&& table = TableBuilder().withNoHeading();
    for (auto&& peer : connectedPeers)
        table.addRow({ peer.name, clientStateToString(peer.state) });

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

void AppStateLobbyBase::selectTab(const tgui::String& tabname)
{
    dic->gui->get<tgui::Tabs>(TABS_ID)->select(tabname);
}

void AppStateLobbyBase::checkMapAvailability()
{
    std::vector<std::string> mapsToDownload =
        lobbySettings.mapSettings
        | std::views::filter(
            [&](const MapSettings& cfg)
            {
                return !std::filesystem::exists(
                    dic->settings->cmdSettings.resourcesDir / "levels"
                    / lobbySettings.packname / cfg.name);
            })
        | std::views::transform([](const MapSettings& cfg) { return cfg.name; })
        | std::ranges::to<std::vector<std::string>>();

    if (!mapsToDownload.empty())
    {
        client->requestMapDownload(lobbySettings.packname, mapsToDownload);
    }
}

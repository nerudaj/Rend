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
    , myPeerData(ClientData {
          .userOpts = dic->settings->player,
      })
    , lobbySettings(LobbySettings {
          .pointlimit = static_cast<int>(dic->settings->cmdSettings.pointlimit),
          .maxNpcs = dic->settings->cmdSettings.maxNpcs })
    , config(config)
{
    dic->logger->ifError(
        0,
        "sendPeerSettingsUpdate",
        client->sendPeerSettingsUpdate(myPeerData));
}

void AppStateLobbyBase::handleNetworkUpdate(const ServerUpdateData& update)
{
    connectedPeers = update.clients;
    if (!update.lobbySettings.isEmpty())
    {
        lobbySettings = update.lobbySettings;
        checkMapAvailability();
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
    const auto mapPackDir =
        dic->settings->cmdSettings.resourcesDir / "levels" / data.mapPackName;
    const auto mapPath = mapPackDir / data.mapName;

    if (std::filesystem::exists(mapPath))
    {
        dic->logger->error(0, "The map at {} already exists", mapPath.string());
        return;
    }

    {
        std::filesystem::create_directory(mapPackDir);
        auto save = std::ofstream(mapPath, std::ios::binary);
        save << base64::from_base64(data.base64encodedMap);
    }

    dic->lateLoadMapIntoManager(mapPath);
    dic->logger->log(0, "Map downloaded and saved to {}", mapPath.string());
}

void AppStateLobbyBase::startGame()
{
    auto&& maplist =
        lobbySettings.mapSettings
        | std::views::filter([](const MapSettings& ms) { return ms.enabled; })
        | std::views::transform([](const MapSettings& ms) { return ms.name; })
        | std::ranges::to<std::vector>();

    if (maplist.empty()) app.popState(ExceptionGameDisconnected::serialize());

    app.pushState<AppStateMapRotationWrapper>(
        dic,
        client,
        GameOptions {
            .players = createPlayerSettings(
                lobbySettings.gameMode == GameMode::SingleFlagCtf),
            .gameMode = lobbySettings.gameMode,
            .pointlimit = static_cast<unsigned>(lobbySettings.pointlimit),
        },
        lobbySettings.packname,
        maplist);
}

std::vector<PlayerOptions>
AppStateLobbyBase::createPlayerSettings(bool isTeamGameMode)
{
    auto&& playerOptions = std::vector<PlayerOptions>();
    for (auto&& [idx, connectedPeer] : std::views::enumerate(connectedPeers))
    {
        playerOptions.push_back(PlayerOptions {
            .kind = idx == client->getMyIndex() ? PlayerKind::LocalHuman
                                                : PlayerKind::RemoteHuman,
            .bindCamera = idx == client->getMyIndex(),
            .name = connectedPeer.userOpts.name,
            .autoswapOnPickup = connectedPeer.userOpts.autoswapOnPickup,
            .team = connectedPeer.userOpts.prefferedTeam,
        });
    }

    const auto&& NPC_NAMES = std::vector<std::string> {
        "[bot] spartan",
        "[bot] phobos",
        "[bot] deimos",
    };

    for (auto&& idx : std::views::iota(size_t { 0 }, lobbySettings.maxNpcs))
    {
        playerOptions.push_back(PlayerOptions {
            .kind = PlayerKind::LocalNpc,
            .bindCamera = false,
            .name = NPC_NAMES.at(idx),
            .team = static_cast<Team>(idx),
        });
    }

    if (playerOptions.size() > 4) playerOptions.resize(4);
    if (isTeamGameMode) adjustTeams(playerOptions);

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
            myPeerData.userOpts = dic->settings->player;
            client->sendPeerSettingsUpdate(myPeerData);
        });
    target->add(formBuilder.build(PANEL_BACKGROUND_COLOR));
}

void AppStateLobbyBase::buildLayoutPlayerTable(tgui::Panel::Ptr target)
{
    auto&& table = TableBuilder().withNoHeading();
    for (auto&& peer : connectedPeers)
        table.addRow({ peer.userOpts.name, clientStateToString(peer.state) });

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
        | std::views::transform(
            [](const MapSettings& cfg) {
                return cfg.name.ends_with(".lvd") ? cfg.name
                                                  : cfg.name + ".lvd";
            })
        | std::views::filter(
            [&](const std::string& name)
            {
                return !std::filesystem::exists(
                    dic->settings->cmdSettings.resourcesDir / "levels"
                    / lobbySettings.packname / name);
            })
        | std::ranges::to<std::vector<std::string>>();

    if (!mapsToDownload.empty())
    {
        dic->logger->ifError(
            0,
            "requestMapDownload",
            client->requestMapDownload(lobbySettings.packname, mapsToDownload));
    }
}

void AppStateLobbyBase::adjustTeams(std::vector<PlayerOptions>& playerOptions)
{
    int sum = 0;
    for (const auto& opt : playerOptions)
    {
        if (opt.team == Team::Red)
            --sum;
        else if (opt.team == Team::Blue)
            ++sum;
    }

    for (auto&& [idx, playerOpts] : std::views::enumerate(playerOptions))
    {
        if ((sum < -1 && playerOpts.team == Team::Red)
            || (playerOpts.team == Team::None && sum <= 0))
        {
            sum += playerOpts.team == Team::None ? 1 : 2;
            playerOpts.team = Team::Blue;
        }
        else if (
            (sum > 1 && playerOpts.team == Team::Blue)
            || (playerOpts.team == Team::None && sum > 0))
        {
            sum -= playerOpts.team == Team::None ? 1 : 2;
            playerOpts.team = Team::Red;
        }
    }
}

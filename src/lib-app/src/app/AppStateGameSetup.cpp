#include <Configs/Strings.hpp>
#include <LevelD.hpp>
#include <app/AppStateGameSetup.hpp>
#include <app/AppStateIngame.hpp>
#include <atomic>
#include <expected>

import Resources;
import FormBuilder;
import WidgetBuilder;
import LayoutBuilder;

std::atomic_bool serverEnabled = true;

void serverLoop(Server server)
{
    while (serverEnabled)
    {
        server.update();
    }
}

AppStateGameSetup::AppStateGameSetup(
    dgm::App& app,
    mem::Rc<const dgm::ResourceManager> resmgr,
    mem::Rc<tgui::Gui> gui,
    mem::Rc<AppOptions> settings,
    mem::Rc<AudioPlayer> audioPlayer,
    mem::Rc<Jukebox> jukebox,
    mem::Rc<PhysicalController> controller) noexcept
    : AppState(app, dgm::AppStateConfig { .clearColor = sf::Color::White })
    , GuiState(gui, audioPlayer)
    , resmgr(resmgr)
    , gui(gui)
    , settings(settings)
    , audioPlayer(audioPlayer)
    , jukebox(jukebox)
    , controller(controller)
    , serverThread(
          serverLoop,
          Server(ServerConfiguration { .port = 10666, .maxClientCount = 4 }))
    , client(mem::Rc<Client>("127.0.0.1", 10666ui16))
    , fraglimit(settings->cmdSettings.fraglimit)
    , playerCount(settings->cmdSettings.playerCount)
    , mapname(settings->cmdSettings.mapname)
{
    buildLayout();
}

void AppStateGameSetup::input()
{
    if (settings->cmdSettings.skipMainMenu)
    {
        settings->cmdSettings.skipMainMenu = false;
        startGame();
    }

    sf::Event event;
    while (app.window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            app.exit();
        }

        gui->handleEvent(event);
    }

    controller->update();
    client->readIncomingPackets(
        std::bind(&AppStateGameSetup::handleNetworkUpdate, this));
}

void AppStateGameSetup::buildLayoutImpl()
{
    mapnames = Filesystem::getLevelNames(
        Filesystem::getLevelsDir(settings->cmdSettings.resourcesDir));

    if (mapname.empty()) mapname = mapnames.front();

    gui->add(
        LayoutBuilder::withBackgroundImage(
            resmgr->get<sf::Texture>("menu_setup.png").value().get())
            .withTitle(Strings::AppState::GameSetup::TITLE, HeadingLevel::H2)
            .withContent(
                FormBuilder()
                    .addOption(
                        Strings::AppState::GameSetup::PLAYER_COUNT,
                        WidgetBuilder::createDropdown(
                            { "1", "2", "3", "4" },
                            std::to_string(playerCount),
                            [this](std::size_t idx) { playerCount = idx + 1; }))
                    .addOption(
                        Strings::AppState::GameSetup::SELECT_MAP,
                        WidgetBuilder::createDropdown(
                            mapnames,
                            mapname,
                            [this](std::size_t idx)
                            { mapname = mapnames[idx]; }))
                    .addOption(
                        Strings::AppState::GameSetup::FRAGLIMIT,
                        WidgetBuilder::createTextInput(
                            std::to_string(fraglimit),
                            [this](const tgui::String& newValue)
                            {
                                if (newValue.empty()) return;
                                fraglimit = std::stoi(std::string(newValue));
                            },
                            WidgetBuilder::getNumericValidator()))
                    .build(PANEL_BACKGROUND_COLOR))
            .withBackButton(WidgetBuilder::createButton(
                Strings::AppState::MainMenu::BACK, [this] { app.popState(); }))
            .withSubmitButton(WidgetBuilder::createButton(
                Strings::AppState::MainMenu::PLAY,
                std::bind(&AppStateGameSetup::commitLobby, this)))
            .build());
}

void AppStateGameSetup::commitLobby()
{
    auto&& result = client->commitLobby();
    if (!result) throw std::runtime_error(result.error());
}

void AppStateGameSetup::handleNetworkUpdate(const ServerUpdateData& update)
{
    // TODO: player settings
    if (update.lobbyCommited)
    {
        mapname = update.mapinfo.name;
        startGame();
    }
}

void AppStateGameSetup::startGame()
{
    auto lvd = LevelD {};
    lvd.loadFromFile(Filesystem::getFullLevelPath(
                         settings->cmdSettings.resourcesDir, mapname)
                         .string());

    app.pushState<AppStateIngame>(
        resmgr,
        gui,
        settings,
        audioPlayer,
        jukebox,
        controller,
        client,
        GameOptions { .players = createPlayerSettings(),
                      .fraglimit = static_cast<unsigned>(fraglimit) },
        lvd);
}

std::vector<PlayerOptions> AppStateGameSetup::createPlayerSettings() const
{
    const auto&& defaultPlayerNames = std::vector<std::string> {
        "player",
        "phobos",
        "spartan",
        "deimos",
    };

    return std::views::iota(0u, playerCount)
           | std::views::transform(
               [&defaultPlayerNames](auto idx)
               {
                   return PlayerOptions { .kind = idx == 0u
                                                      ? PlayerKind::LocalHuman
                                                      : PlayerKind::LocalNpc,
                                          .bindCamera = idx == 0,
                                          .name = defaultPlayerNames[idx] };
               })
           | std::ranges::to<std::vector<PlayerOptions>>();
}

void AppStateGameSetup::cleanup()
{
    serverEnabled = false;
    serverThread.join();
    app.popState();
}

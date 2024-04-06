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
    , client(mem::Rc<Client>("127.0.0.1", 10666ui16))
    , lobbySettings(LobbySettings {
          .fraglimit = static_cast<int>(settings->cmdSettings.fraglimit),
          .playerCount = settings->cmdSettings.playerCount })
    , mapPackNames(Filesystem::getLevelPackNames(
          Filesystem::getLevelsDir(settings->cmdSettings.resourcesDir)))
{
    guiWithModals->gui.setTarget(app.window.getWindowContext());
    guiWithModals->gui.setFont(
        resmgr->get<tgui::Font>("pico-8.ttf").value().get());
    selectMapPack(mapPackNames.front());
    buildLayout();
    client->sendLobbyUpdate(lobbySettings);
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

        guiWithModals->gui.handleEvent(event);
        gui->handleEvent(event);
    }

    controller->update();
    client->readIncomingPackets(std::bind(
        &AppStateGameSetup::handleNetworkUpdate, this, std::placeholders::_1));
}

void AppStateGameSetup::buildLayoutImpl()
{
    gui->add(
        LayoutBuilder::withBackgroundImage(
            resmgr->get<sf::Texture>("menu_setup.png").value().get())
            .withTitle(Strings::AppState::GameSetup::TITLE, HeadingLevel::H1)
            .withContent(
                FormBuilder()
                    .addOption(
                        Strings::AppState::GameSetup::PLAYER_COUNT,
                        WidgetBuilder::createDropdown(
                            { "1", "2", "3", "4" },
                            std::to_string(lobbySettings.playerCount),
                            [this](std::size_t idx)
                            {
                                lobbySettings.playerCount = idx + 1;
                                client->sendLobbyUpdate(lobbySettings);
                            }))
                    .addOption(
                        Strings::AppState::GameSetup::SELECT_PACK,
                        WidgetBuilder::createDropdown(
                            mapPackNames,
                            lobbySettings.packname,
                            [this](std::size_t idx) {
                                selectMapPackAndSendUpdate(
                                    mapPackNames.at(idx));
                            }))
                    .addOption(
                        Strings::AppState::GameSetup::SELECT_MAPS,
                        WidgetBuilder::createButton(
                            Strings::AppState::GameSetup::DOTDOTDOT,
                            std::bind(&AppStateGameSetup::openMapPicker, this)))
                    .addOption(
                        Strings::AppState::GameSetup::FRAGLIMIT,
                        WidgetBuilder::createTextInput(
                            std::to_string(lobbySettings.fraglimit),
                            [this](const tgui::String& newValue)
                            {
                                if (newValue.empty()) return;
                                lobbySettings.fraglimit =
                                    std::stoi(std::string(newValue));
                                client->sendLobbyUpdate(lobbySettings);
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
    lobbySettings = update.lobbySettings;

    if (update.lobbyCommited)
    {
        startGame();
    }
}

void AppStateGameSetup::startGame()
{
    auto firstEnabled = std::find_if(
        lobbySettings.mapSettings.begin(),
        lobbySettings.mapSettings.end(),
        [](const MapSettings& ms) { return ms.enabled; });

    if (firstEnabled == lobbySettings.mapSettings.end())
        throw std::runtime_error("No map selected!");

    auto lvd = LevelD {};
    lvd.loadFromFile(Filesystem::getFullLevelPath(
                         settings->cmdSettings.resourcesDir,
                         lobbySettings.packname,
                         firstEnabled->name)
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
                      .fraglimit =
                          static_cast<unsigned>(lobbySettings.fraglimit) },
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

    return std::views::iota(0u, lobbySettings.playerCount)
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

void AppStateGameSetup::selectMapPack(const std::string& packname)
{
    lobbySettings.packname = packname;
    lobbySettings.mapSettings =
        Filesystem::getLevelNames(
            Filesystem::getLevelsDir(settings->cmdSettings.resourcesDir),
            packname)
        | std::views::transform(
            [](const std::string& name)
            { return MapSettings { .name = name, .enabled = true }; })
        | std::ranges::to<std::vector>();
    lobbySettings.mapOrder =
        std::views::iota(size_t { 0 }, lobbySettings.mapSettings.size())
        | std::ranges::to<std::vector>();
}

void AppStateGameSetup::selectMapPackAndSendUpdate(const std::string& packname)
{
    selectMapPack(packname);
    client->sendLobbyUpdate(lobbySettings);
}

void AppStateGameSetup::openMapPicker()
{
    mapPickerDialog = mem::Box<MapPickerDialog>(
        guiWithModals,
        lobbySettings.mapSettings
            | std::views::transform(
                [](auto s) {
                    return MapSettingsForPicker { s.name, s.enabled };
                })
            | std::ranges::to<std::vector>());
    mapPickerDialog->open(
        std::bind(&AppStateGameSetup::handleMapRotationUpdate, this));
}

void AppStateGameSetup::handleMapRotationUpdate()
{
    lobbySettings.mapSettings =
        mapPickerDialog->getMapSettings()
        | std::views::transform(
            [](auto s) {
                return MapSettings { s.name, s.enabled };
            })
        | std::ranges::to<std::vector>();
    client->sendLobbyUpdate(lobbySettings);
}

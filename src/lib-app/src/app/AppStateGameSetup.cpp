#include <Configs/Strings.hpp>
#include <LevelD.hpp>
#include <app/AppStateGameSetup.hpp>
#include <app/AppStateIngame.hpp>

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
}

void AppStateGameSetup::buildLayoutImpl()
{
    mapnames = Filesystem::getLevelNames(
        Filesystem::getLevelsDir(settings->cmdSettings.resourcesDir));

    if (mapname.empty()) mapname = mapnames.front();

    auto panel =
        WidgetBuilder::createPanel({ "100%", "100%" }, PANEL_BACKGROUND_COLOR);
    FormBuilder(panel)
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
                [this](std::size_t idx) { mapname = mapnames[idx]; }))
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
        .build();

    gui->add(
        LayoutBuilder::withBackgroundImage(
            resmgr->get<sf::Texture>("menu_setup.png").value().get())
            .withTitle(Strings::AppState::GameSetup::TITLE, HeadingLevel::H2)
            .withContent(panel)
            .withBackButton(WidgetBuilder::createButton(
                Strings::AppState::MainMenu::BACK, [this] { app.popState(); }))
            .withSubmitButton(WidgetBuilder::createButton(
                Strings::AppState::MainMenu::PLAY,
                std::bind(&AppStateGameSetup::startGame, this)))
            .build());
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
        GameOptions { .players = createPlayerSettings(),
                      .fraglimit = static_cast<unsigned>(fraglimit) },
        lvd);
}

std::vector<PlayerOptions> AppStateGameSetup::createPlayerSettings() const
{
    return std::views::iota(0u, playerCount)
           | std::views::transform(
               [](auto idx)
               {
                   return PlayerOptions { .kind = idx == 0u
                                                      ? PlayerKind::LocalHuman
                                                      : PlayerKind::LocalNpc,
                                          .bindCamera = idx == 0 };
               })
           | std::ranges::to<std::vector<PlayerOptions>>();
}

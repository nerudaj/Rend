#include <Configs/Strings.hpp>
#include <LevelD.hpp>
#include <app/AppStateGameSetup.hpp>
#include <app/AppStateIngame.hpp>

import Resources;
import FormBuilder;
import WidgetBuilder;

AppStateGameSetup::AppStateGameSetup(
    dgm::App& app,
    mem::Rc<const dgm::ResourceManager> resmgr,
    mem::Rc<tgui::Gui> gui,
    mem::Rc<AppOptions> settings,
    mem::Rc<AudioPlayer> audioPlayer,
    mem::Rc<Jukebox> jukebox) noexcept
    : AppState(app)
    , GuiState(gui, audioPlayer)
    , resmgr(resmgr)
    , gui(gui)
    , settings(settings)
    , audioPlayer(audioPlayer)
    , jukebox(jukebox)
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
}

void AppStateGameSetup::buildLayoutImpl()
{
    gui->add(createH2Title("game setup"));

    auto mapnames = Filesystem::getLevelNames(
        Filesystem::getLevelsDir(settings->cmdSettings.resourcesDir));

    if (mapname.empty()) mapname = mapnames.front();

    auto panel = tgui::Panel::create({ "60%", "50%" });
    panel->setPosition("20%", "35%");
    gui->add(panel);

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
                [this, &mapnames](std::size_t idx)
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
        .build();

    gui->add(createButton(
        "start game",
        { "79%", "94%" },
        { "20%", "5%" },
        std::bind(&AppStateGameSetup::startGame, this)));
    gui->add(createBackButton([this] { app.popState(); }));
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
                   return PlayerOptions { .kind = idx == 0
                                                      ? PlayerKind::LocalHuman
                                                      : PlayerKind::LocalNpc,
                                          .bindCamera = idx == 0 };
               })
           | std::ranges::to<std::vector<PlayerOptions>>();
}

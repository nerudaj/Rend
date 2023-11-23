#include <LevelD.hpp>
#include <app/AppStateGameSetup.hpp>
#include <app/AppStateIngame.hpp>

import Resources;

AppStateGameSetup::AppStateGameSetup(
    dgm::App& app,
    mem::Rc<const dgm::ResourceManager> resmgr,
    mem::Rc<tgui::Gui> gui,
    mem::Rc<AppOptions> settings,
    mem::Rc<AudioPlayer> audioPlayer) noexcept
    : AppState(app)
    , GuiState(gui, audioPlayer)
    , resmgr(resmgr)
    , gui(gui)
    , settings(settings)
    , audioPlayer(audioPlayer)
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

    getCommonLayoutBuilder(3)
        .addOption(
            "player count",
            "PlayerCount",
            WidgetCreator::createDropdown(
                { "1", "2", "3", "4" },
                std::to_string(playerCount),
                [this]
                {
                    auto dropdown = gui->get<tgui::ComboBox>("PlayerCount");
                    playerCount =
                        std::stoul(std::string(dropdown->getSelectedItem()));
                }))
        .addOption(
            "select map",
            "MapnameDropdown",
            WidgetCreator::createDropdown(
                mapnames,
                mapname,
                [this]
                {
                    auto dropdown = gui->get<tgui::ComboBox>("MapnameDropdown");
                    mapname = dropdown->getSelectedItem().toStdString();
                }))
        .addOption(
            "fraglimit",
            "FraglimitInput",
            WidgetCreator::createTextInput(
                [this](const tgui::String& newValue)
                {
                    if (newValue.empty()) return;
                    fraglimit = std::stoi(std::string(newValue));
                },
                std::to_string(fraglimit),
                WidgetCreator::getNumericValidator()))
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
        GameOptions { .players = createPlayerSettings(),
                      .fraglimit = static_cast<unsigned>(fraglimit) },
        lvd);
}

std::vector<PlayerOptions> AppStateGameSetup::createPlayerSettings() const
{
    std::vector<PlayerOptions> result;

    for (unsigned i = 0; i < playerCount; i++)
    {
        result.push_back(PlayerOptions { .kind = i == 0 ? PlayerKind::LocalHuman
                                                        : PlayerKind::LocalNpc,
                                         .bindCamera = i == 0 });
    }

    return result;
}

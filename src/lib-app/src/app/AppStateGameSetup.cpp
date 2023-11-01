#include "GameSettings.hpp"
#include <LevelD.hpp>
#include <app/AppStateGameSetup.hpp>
#include <app/AppStateIngame.hpp>

AppStateGameSetup::AppStateGameSetup(
    dgm::App& app,
    mem::Rc<const dgm::ResourceManager> resmgr,
    mem::Rc<tgui::Gui> gui,
    mem::Rc<Settings> settings,
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
    using namespace std::views;

    gui->add(createH2Title("game setup"));

    auto sanitizeMapName = [](const std::string& str)
    { return str.substr(0, str.size() - 4); };
    auto mapnames = resmgr->getLoadedResourceIds<LevelD>().value()
                    | transform(sanitizeMapName)
                    | std::ranges::to<std::vector<std::string>>();

    getCommonLayoutBuilder(3)
        .addOption(
            "player count",
            "PlayerCount",
            WidgetCreator::createDropdown(
                { "2", "3", "4" },
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
                sanitizeMapName(mapname.empty() ? mapnames.front() : mapname),
                [this]
                {
                    auto dropdown = gui->get<tgui::ComboBox>("MapnameDropdown");
                    mapname = std::string(dropdown->getSelectedItem() + ".lvd");
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
    app.pushState<AppStateIngame>(
        resmgr,
        gui,
        settings,
        audioPlayer,
        GameSettings { .map = settings->cmdSettings.mapname,
                       .players = createPlayerSettings(),
                       .fraglimit = fraglimit });
}

std::vector<PlayerSettings> AppStateGameSetup::createPlayerSettings() const
{
    std::vector<PlayerSettings> result;

    for (unsigned i = 0; i < playerCount; i++)
    {
        result.push_back(PlayerSettings {
            .kind = i == 0 ? PlayerKind::LocalHuman : PlayerKind::LocalNpc,
            .bindCamera = i == 0 });
    }

    return result;
}

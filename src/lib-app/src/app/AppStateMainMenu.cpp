#include "app/AppStateMainMenu.hpp"
#include "app/AppStateIngame.hpp"
#include "app/AppStateMenuOptions.hpp"
#include "settings/GameTitle.hpp"

void AppStateMainMenu::buildLayoutImpl()
{
    auto title = createWindowTitle({ "0%", "5%" }, { "100%", "25%" }, "rend");
    title->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    title->setTextSize(72);
    gui->add(title);

    auto layout = tgui::VerticalLayout::create({ "15%", "30%" });
    layout->setPosition("42.5%", "35%");
    gui->add(layout);

    createButtonListInLayout(
        layout,
        { ButtonProps("play", [this] { startGame(); }),
          ButtonProps(
              "options",
              [this] {
                  app.pushState<AppStateMenuOptions>(
                      gui, audioPlayer, settings);
              }),
          ButtonProps("exit", [this] { app.exit(); }) },
        0.05f);
}

void AppStateMainMenu::input()
{
    if (settings->cmdSettings.skipMainMenu)
    {
        startGame();
        settings->cmdSettings.skipMainMenu = false;
    }

    sf::Event event;
    while (app.window.pollEvent(event))
    {
        gui->handleEvent(event);
    }
}

void AppStateMainMenu::startGame()
{
    app.pushState<AppStateIngame>(
        resmgr,
        gui,
        settings,
        audioPlayer,
        GameSettings { .map = settings->cmdSettings.mapname,
                       .players = createPlayerSettings(
                           settings->cmdSettings.playerCount) });
}

std::vector<PlayerSettings>
AppStateMainMenu::createPlayerSettings(unsigned playerCount)
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

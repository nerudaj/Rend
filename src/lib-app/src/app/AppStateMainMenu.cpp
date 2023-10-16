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
        { ButtonProps(
              "play",
              [this] {
                  app.pushState<AppStateIngame>(
                      resmgr, gui, settings, audioPlayer);
              }),
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
        app.pushState<AppStateIngame>(resmgr, gui, settings, audioPlayer);
        settings->cmdSettings.skipMainMenu = false;
    }

    sf::Event event;
    while (app.window.pollEvent(event))
    {
        gui->handleEvent(event);
    }
}

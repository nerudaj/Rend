#include "app/AppStateMainMenu.hpp"
#include "app/AppStateGameSetup.hpp"
#include "app/AppStateIngame.hpp"
#include "app/AppStateMenuOptions.hpp"
#include "settings/GameTitle.hpp"

void AppStateMainMenu::buildLayoutImpl()
{
    auto panel = tgui::Panel::create();
    panel->getRenderer()->setTextureBackground(
        resmgr->get<sf::Texture>("titlebgr.png").value().get());
    gui->add(panel);

    gui->add(createH1Title("rend"));

    auto layout = tgui::VerticalLayout::create({ "15%", "30%" });
    layout->setPosition("42.5%", "35%");
    gui->add(layout);

    createButtonListInLayout(
        layout,
        { ButtonProps("play", [this] { goToGameSetup(); }),
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
        goToGameSetup();
    }

    sf::Event event;
    while (app.window.pollEvent(event))
    {
        gui->handleEvent(event);
    }
}

void AppStateMainMenu::goToGameSetup()
{
    app.pushState<AppStateGameSetup>(resmgr, gui, settings, audioPlayer);
}

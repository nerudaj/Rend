#include "app/AppStatePaused.hpp"
#include "app/AppStateMenuOptions.hpp"

void AppStatePaused::buildLayoutImpl()
{
    auto panel = tgui::Panel::create({ "20%", "50%" });
    panel->getRenderer()->setBackgroundColor(sf::Color(192, 192, 192));
    panel->setPosition({ "40%", "25%" });
    gui->add(panel);

    auto layout = tgui::VerticalLayout::create({ "90%", "90%" });
    layout->setPosition("5%", "5%");
    panel->add(layout);

    createButtonListInLayout(
        layout,
        { ButtonProps("resume", [&] { app.popState(); }),
          ButtonProps(
              "options",
              [&]
              {
                  app.pushState<AppStateMenuOptions>(
                      gui,
                      resmgr,
                      audioPlayer,
                      jukebox,
                      settings,
                      controller,
                      true);
              }),
          ButtonProps(
              " main menu ", [&] { app.popState("pop if not menu", 2); }),
          ButtonProps("exit", [&] { app.exit(); }) },
        0.05f);
}

void AppStatePaused::input()
{
    sf::Event event;
    while (app.window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            app.exit();
        }
        else if (controller->isEscapePressed())
        {
            app.popState();
        }

        gui->handleEvent(event);
    }

    controller->update();
}

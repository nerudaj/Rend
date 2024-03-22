#include "app/AppStatePaused.hpp"
#include "app/AppStateMenuOptions.hpp"

import GuiBuilder;
import AppMessage;

void AppStatePaused::buildLayoutImpl()
{
    gui->add(
        LayoutBuilder::withNoBackgroundImage()
            .withTitle(Strings::AppState::Pause::TITLE, HeadingLevel::H1)
            .withContent(
                ButtonListBuilder()
                    .addButton(
                        Strings::AppState::Pause::RESUME,
                        [&] { app.popState(); })
                    .addButton(
                        Strings::AppState::MainMenu::OPTIONS,
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
                        })
                    .addButton(
                        Strings::AppState::Pause::TO_MENU,
                        [&] { app.popState(PopIfNotMainMenu::serialize()); })
                    .addButton(
                        Strings::AppState::MainMenu::EXIT, [&] { app.exit(); })
                    .build())
            .withNoBackButton()
            .withNoSubmitButton()
            .build());
}

void AppStatePaused::input()
{
    if (!hasFocus) return;

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

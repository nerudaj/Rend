#include "app/AppStatePaused.hpp"
#include "GuiBuilder.hpp"
#include "app/AppStateMenuOptions.hpp"
#include "utils/AppMessage.hpp"

void AppStatePaused::buildLayoutImpl()
{
    dic->gui->add(
        LayoutBuilder::withNoBackgroundImage()
            .withTitle(Strings::AppState::Pause::TITLE, HeadingLevel::H1)
            .withContent(
                ButtonListBuilder()
                    .addButton(
                        Strings::AppState::Pause::RESUME,
                        [&] { app.popState(); })
                    .addButton(
                        Strings::AppState::MainMenu::OPTIONS,
                        [&] { app.pushState<AppStateMenuOptions>(dic, true); })
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
        else if (dic->controller->isEscapePressed())
        {
            app.popState();
        }

        dic->gui->gui.handleEvent(event);
    }

    dic->controller->update();
}

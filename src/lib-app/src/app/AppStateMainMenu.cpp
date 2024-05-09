#include "app/AppStateMainMenu.hpp"
#include "Configs/Strings.hpp"
#include "GuiBuilder.hpp"
#include "Utilities/TguiHelper.hpp"
#include "app/AppStateIngame.hpp"
#include "app/AppStateMenuOptions.hpp"
#include <app/AppStateServerWrapper.hpp>

void AppStateMainMenu::buildLayoutImpl()
{
    dic->gui->add(
        LayoutBuilder::withBackgroundImage(
            dic->resmgr->get<sf::Texture>("menu_title.png").value().get())
            .withTitle(Strings::TITLE, HeadingLevel::H1)
            .withContent(ButtonListBuilder()
                             .addButton(
                                 Strings::AppState::MainMenu::PLAY,
                                 [this] { goToGameSetup(); })
                             .addButton(
                                 Strings::AppState::MainMenu::EDITOR,
                                 [this] {
                                     app.pushState<AppStateServerWrapper>(
                                         dic, ServerWrapperTarget::Editor);
                                 })
                             .addButton(
                                 Strings::AppState::MainMenu::OPTIONS,
                                 [this]
                                 { app.pushState<AppStateMenuOptions>(dic); })
                             .addButton(
                                 Strings::AppState::MainMenu::EXIT,
                                 [this] { app.exit(); })
                             .build())
            .withNoBackButton()
            .withNoSubmitButton()
            .build());
}

void AppStateMainMenu::input()
{
    if (dic->settings->cmdSettings.skipMainMenu)
    {
        goToGameSetup();
    }

    sf::Event event;
    while (app.window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed) app.exit();

        dic->gui->gui.handleEvent(event);
    }

    dic->controller->update();
}

void AppStateMainMenu::goToGameSetup()
{
    app.pushState<AppStateServerWrapper>(dic, ServerWrapperTarget::GameSetup);
}

#include "app/AppStateMainMenu.hpp"
#include "Configs/Strings.hpp"
#include "Dialogs/ErrorInfoDialog.hpp"
#include "Dialogs/YesNoCancelDialog.hpp"
#include "Shortcuts/ShortcutEngine.hpp"
#include "Utilities/TguiHelper.hpp"
#include "app/AppStateEditor.hpp"
#include "app/AppStateGameSetup.hpp"
#include "app/AppStateIngame.hpp"
#include "app/AppStateMenuOptions.hpp"
#include "settings/GameTitle.hpp"

import GuiBuilder;

void AppStateMainMenu::buildLayoutImpl()
{
    gui->add(LayoutBuilder::withBackgroundImage(
                 resmgr->get<sf::Texture>("menu_title.png").value().get())
                 .withTitle("rend", HeadingLevel::H1)
                 .withContent(
                     ButtonListBuilder()
                         .addButton(
                             Strings::AppState::MainMenu::PLAY,
                             [this] { goToGameSetup(); })
                         .addButton(
                             Strings::AppState::MainMenu::EDITOR,
                             [this]
                             {
                                 auto _gui = mem::Rc<Gui>();
                                 app.pushState<AppStateEditor>(
                                     gui,
                                     _gui,
                                     resmgr,
                                     settings,
                                     audioPlayer,
                                     jukebox,
                                     controller,
                                     mem::Rc<ShortcutEngine>(
                                         [_gui]
                                         { return _gui->isAnyModalOpened(); }),
                                     mem::Rc<YesNoCancelDialog>(_gui),
                                     mem::Rc<ErrorInfoDialog>(_gui));
                             })
                         .addButton(
                             Strings::AppState::MainMenu::OPTIONS,
                             [this]
                             {
                                 app.pushState<AppStateMenuOptions>(
                                     gui,
                                     resmgr,
                                     audioPlayer,
                                     jukebox,
                                     settings,
                                     controller);
                             })
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
    if (settings->cmdSettings.skipMainMenu)
    {
        goToGameSetup();
    }

    sf::Event event;
    while (app.window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed) app.exit();

        gui->handleEvent(event);
    }

    controller->update();
}

void AppStateMainMenu::goToGameSetup()
{
    app.pushState<AppStateGameSetup>(
        resmgr, gui, settings, audioPlayer, jukebox, controller);
}

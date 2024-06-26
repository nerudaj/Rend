#include "app/AppStateJoinGame.hpp"
#include "LayoutBuilder.hpp"
#include "app/AppStatePeerLobby.hpp"
#include "utils/AppMessage.hpp"
#include "utils/InputHandler.hpp"
#include <Configs/Strings.hpp>
#include <CoreTypes.hpp>
#include <FormBuilder.hpp>
#include <SFML/Network.hpp>
#include <WidgetBuilder.hpp>

void AppStateJoinGame::input()
{
    InputHandler::handleUiStateInput(app, *dic);
}

void AppStateJoinGame::buildLayout()
{
    dic->gui->rebuildWith(
        LayoutBuilder()
            .withBackgroundImage(
                dic->resmgr->get<sf::Texture>("menu_setup.png").value().get())
            .withTitle(Strings::AppState::JoinGame::TITLE, HeadingLevel::H1)
            .withContent(FormBuilder()
                             .addOption(
                                 Strings::AppState::JoinGame::INPUT_IP,
                                 WidgetBuilder::createTextInput(
                                     ipAddressInput,
                                     [&](tgui::String text)
                                     { ipAddressInput = text.toStdString(); }))
                             .build(PANEL_BACKGROUND_COLOR))
            .withBackButton(WidgetBuilder::createButton(
                Strings::AppState::MainMenu::BACK, [&] { app.popState(); }))
            .withSubmitButton(WidgetBuilder::createButton(
                Strings::AppState::JoinGame::CONFIRM, [&] { tryToJoin(); }))
            .build());
}

void AppStateJoinGame::restoreFocusImpl(const std::string& message)
{
    buildLayout();
    handleAppMessage<AppStateJoinGame>(app, message);
}

void AppStateJoinGame::tryToJoin()
{
    try
    {
        app.pushState<AppStatePeerLobby>(dic, ipAddressInput);
    }
    catch (const std::exception& e)
    {
        warningDialog.open(e.what());
    }
}

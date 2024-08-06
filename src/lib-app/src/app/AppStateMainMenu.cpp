#include "app/AppStateMainMenu.hpp"
#include "Configs/Strings.hpp"
#include "GuiBuilder.hpp"
#include "Utilities/TguiHelper.hpp"
#include "app/AppStateIngame.hpp"
#include "app/AppStateMenuOptions.hpp"
#include "app/AppStatePeerLobby.hpp"
#include "app/AppStateServerWrapper.hpp"
#include "utils/InputHandler.hpp"

void AppStateMainMenu::buildLayout()
{
    dic->gui->rebuildWith(
        LayoutBuilder::withBackgroundImage(
            dic->resmgr->get<sf::Texture>("menu_title.png").value().get())
            .withTitle(Strings::TITLE, HeadingLevel::H1)
            .withContent(ButtonListBuilder()
                             .addButton(
                                 Strings::AppState::MainMenu::PLAY,
                                 [this] { goToGameSetup(); })
                             .addButton(
                                 Strings::AppState::MainMenu::JOIN,
                                 [this] { joinGameDialog.open([] {}); })
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

void AppStateMainMenu::restoreFocusImpl(const std::string& message)
{
    auto&& result = handleAppMessage<AppStateMainMenu>(app, message);
    app.window.getWindowContext().setTitle(Strings::CAPTITLE);
    dic->jukebox->playTitleSong();
    buildLayout();

    if (result) dic->warningDialog.open(result.value());
}

void AppStateMainMenu::input()
{
    if (dic->settings->cmdSettings.skipMainMenu)
    {
        goToGameSetup();
    }

    try
    {
        InputHandler::handleUiStateInputWithoutGoBackOption(app, *dic);
    }
    catch (const std::exception& e)
    {
        dic->warningDialog.open(e.what());
    }
}

void AppStateMainMenu::goToGameSetup()
{
    app.pushState<AppStateServerWrapper>(dic, ServerWrapperTarget::GameSetup);
}

void AppStateMainMenu::tryJoinGame(const std::string& ip)
{
    app.pushState<AppStatePeerLobby>(dic, ip);
    dic->settings->network.joinIpAddress = ip;
}

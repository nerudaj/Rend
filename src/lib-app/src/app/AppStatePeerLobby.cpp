#include "app/AppStatePeerLobby.hpp"
#include "utils/AppMessage.hpp"
#include "utils/InputHandler.hpp"
#include <Configs/Strings.hpp>
#include <CoreTypes.hpp>
#include <GuiBuilder.hpp>

void AppStatePeerLobby::input()
{
    InputHandler::handleUiStateInput(app, *dic);

    // TODO: handle input network packets
}

void AppStatePeerLobby::buildLayout()
{
    dic->gui->rebuildWith(
        LayoutBuilder()
            .withBackgroundImage(
                dic->resmgr->get<sf::Texture>("menu_setup.png").value().get())
            .withTitle(Strings::AppState::PeerLobby::TITLE, HeadingLevel::H1)
            .withContent(FormBuilder().build())
            .withBackButton(WidgetBuilder::createButton(
                Strings::AppState::MainMenu::BACK, [&] { app.popState(); }))
            .withNoSubmitButton()
            .build());
}

void AppStatePeerLobby::restoreFocusImpl(const std::string& message)
{
    buildLayout();
    handleAppMessage<decltype(this)>(app, message);
}

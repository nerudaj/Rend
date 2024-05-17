#include "app/AppStatePaused.hpp"
#include "GuiBuilder.hpp"
#include "app/AppStateMenuOptions.hpp"
#include "utils/AppMessage.hpp"
#include "utils/InputHandler.hpp"
#include <Configs/Strings.hpp>
#include <CoreTypes.hpp>

void AppStatePaused::buildLayout()
{
    dic->gui->rebuildWith(
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

void AppStatePaused::restoreFocusImpl(const std::string& message)
{
    buildLayout();
    handleAppMessage<AppStatePaused>(app, message);
}

void AppStatePaused::input()
{
    InputHandler::handleUiStateWithFocus(hasFocus, app, *dic);
}

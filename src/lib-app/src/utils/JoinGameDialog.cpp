#include "utils/JoinGameDialog.hpp"
#include <Configs/Strings.hpp>
#include <FormBuilder.hpp>
#include <WidgetBuilder.hpp>

JoinGameDialog::JoinGameDialog(
    mem::Rc<Gui> gui,
    const std::string& defaultIp,
    std::function<void(const std::string& ip)> tryJoinGame)
    : DialogInterface(
        gui, "JoinGameDialogId", Strings::AppState::JoinGame::TITLE)
    , joinIp(defaultIp)
    , tryJoinGame(tryJoinGame)
{
}

std::expected<ReturnFlag, ErrorMessage>
JoinGameDialog::validateBeforeConfirmation() const
{
    try
    {
        tryJoinGame(joinIp);
        return ReturnFlag::Success;
    }
    catch (const std::exception& e)
    {
        return std::unexpected(e.what());
    }
}

void JoinGameDialog::buildLayoutImpl(tgui::Panel::Ptr target)
{
    constexpr const char* TEXT_INPUT_ID = "JoinGameInputId";

    target->add(
        FormBuilder()
            .addOptionWithWidgetId(
                Strings::AppState::JoinGame::INPUT_IP,
                WidgetBuilder::createTextInput(
                    joinIp,
                    [&](tgui::String text) { joinIp = text.toStdString(); }),
                TEXT_INPUT_ID)
            .build(PANEL_BACKGROUND_COLOR));

    auto input = gui->get<tgui::EditBox>(TEXT_INPUT_ID);
    input->setFocused(true);
}

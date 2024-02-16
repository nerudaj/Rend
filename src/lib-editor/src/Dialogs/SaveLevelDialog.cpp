#include "Dialogs/SaveLevelDialog.hpp"
#include "Configs/Strings.hpp"

constexpr const char* LEVEL_NAME_ID = "LevelNameId";

SaveLevelDialog::SaveLevelDialog(mem::Rc<Gui> gui)
    : DialogInterface(
        gui,
        "SaveLevelDialogId",
        Strings::Dialog::Title::SAVE_LEVEL,
        std::vector<OptionLine> { OptionInput {
            Strings::Dialog::Body::LEVEL_NAME, LEVEL_NAME_ID, "" } })
{
}

std::string SaveLevelDialog::getLevelName() const
{
    return gui->get<tgui::EditBox>(LEVEL_NAME_ID)->getText().toStdString();
}

import GuiBuilder;

NewSaveLevelDialog::NewSaveLevelDialog(
    mem::Rc<Gui> gui, const std::filesystem::path& levelsDir)
    : ModernDialogInterface(
        gui, "SaveLevelDialogId", Strings::Dialog::Title::SAVE_LEVEL)
    , levelsDir(levelsDir)
{
}

void NewSaveLevelDialog::buildLayoutImpl(tgui::Panel::Ptr target)
{
    FormBuilder(target)
        .addOption(
            Strings::Dialog::Body::LEVEL_NAME,
            WidgetBuilder::createTextInput(
                levelName,
                [&](tgui::String str) { levelName = str.toStdString(); },
                "[a-zA-Z0-9_]+"))
        .build();
}

std::expected<ReturnFlag, ErrorMessage>
NewSaveLevelDialog::validateBeforeConfirmation() const
{
    if (levelName.empty())
        return std::unexpected(
            std::string(Strings::Dialog::Message::LEVEL_NAME_EMPTY));

    for (auto&& path : std::filesystem::directory_iterator(levelsDir))
    {
        auto filename = path.path().stem();
        if (filename == levelName)
            return std::unexpected(
                std::string(Strings::Dialog::Message::LEVEL_NAME_TAKEN));
    }

    return ReturnFlag::Success;
}

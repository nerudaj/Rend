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

#include "Dialogs/NewLevelDialog.hpp"
#include "Globals.hpp"

constexpr const char* INPUT_LEVEL_WIDTH_ID = "InputLevelWidth";
constexpr const char* INPUT_LEVEL_HEIGHT_ID = "InputLevelHeight";

void NewLevelDialog::customOpenCode() {}

unsigned NewLevelDialog::getLevelWidth() const
{
    return std::stoul(gui->get<tgui::ComboBox>(INPUT_LEVEL_WIDTH_ID)
                          ->getSelectedItem()
                          .toStdString());
}

unsigned NewLevelDialog::getLevelHeight() const
{
    return std::stoul(gui->get<tgui::ComboBox>(INPUT_LEVEL_HEIGHT_ID)
                          ->getSelectedItem()
                          .toStdString());
}

NewLevelDialog::NewLevelDialog(
    mem::Rc<Gui> gui, mem::Rc<FileApiInterface> fileApi)
    : DialogInterface(
        gui,
        "ModalNewLevel",
        "New level",
        std::vector<OptionLine> { {
            OptionDropdown {
                "Level width:", INPUT_LEVEL_WIDTH_ID, { "16", "32" } },
            OptionDropdown {
                "Level height:", INPUT_LEVEL_HEIGHT_ID, { "16", "32" } },
        } })
    , configPath(configPath)
{
}

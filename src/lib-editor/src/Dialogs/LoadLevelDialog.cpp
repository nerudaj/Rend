#include "Dialogs/LoadLevelDialog.hpp"
#include "Configs/Strings.hpp"
#include "Filesystem.hpp"
#include <filesystem>

constexpr const char* SELECT_LEVEL_ID = "SelectLevelId";

LoadLevelDialog::LoadLevelDialog(
    mem::Rc<Gui> gui, const std::filesystem::path& rootDir)
    : DialogInterface(
        gui,
        "LoadLevelDialog",
        Strings::Dialog::Title::OPEN_LEVEL,
        std::vector<OptionLine> { { OptionDropdown {
            Strings::Dialog::Body::SELECT_LEVEL,
            SELECT_LEVEL_ID,
            Filesystem::getLevelNames(Filesystem::getLevelsDir(rootDir)) } } })
{
}

std::string LoadLevelDialog::getLevelName() const
{
    auto combo = gui->get<tgui::ComboBox>(SELECT_LEVEL_ID);
    return combo->getSelectedItem().toStdString();
}

void LoadLevelDialog::customOpenCode() {}

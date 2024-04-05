#include "Dialogs/LoadLevelDialog.hpp"
#include "Configs/Strings.hpp"

import Resources;
import FormBuilder;
import WidgetBuilder;

constexpr const char* SELECT_LEVEL_ID = "SelectLevelId";

LoadLevelDialog::LoadLevelDialog(
    mem::Rc<Gui> gui, const std::filesystem::path& rootDir)
    : ModernDialogInterface(
        gui, "LoadLevelDialog", Strings::Dialog::Title::OPEN_LEVEL)
    , levelsDir(Filesystem::getLevelsDir(rootDir))
    , mapPackNames(Filesystem::getLevelPackNames(levelsDir))
    , mapPackName(mapPackNames.front())
    , mapNames(Filesystem::getLevelNames(levelsDir, mapPackName))
    , mapName(mapNames.front())
    , gui(gui)
{
}

void LoadLevelDialog::buildLayoutImpl(tgui::Panel::Ptr panel)
{
    panel->add(
        FormBuilder()
            .addOption(
                Strings::Dialog::Body::SELECT_PACK,
                WidgetBuilder::createDropdown(
                    mapPackNames,
                    mapPackName,
                    std::bind(
                        &LoadLevelDialog::handleSelectedMapPack,
                        this,
                        std::placeholders::_1)))
            .addOptionWithWidgetId(
                Strings::Dialog::Body::SELECT_LEVEL,
                WidgetBuilder::createDropdown(
                    mapNames,
                    mapName,
                    [&](const size_t idx) { mapName = mapNames.at(idx); }),
                SELECT_LEVEL_ID)
            .build());
}

void LoadLevelDialog::handleSelectedMapPack(const size_t idx)
{
    mapPackName = mapPackNames.at(idx);
    mapNames = Filesystem::getLevelNames(levelsDir, mapPackName);
    WidgetBuilder::updateDropdownItems(
        gui->get<tgui::ComboBox>(SELECT_LEVEL_ID), mapNames);
}

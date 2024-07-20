#include "Dialogs/LoadLevelDialog.hpp"
#include "Configs/Strings.hpp"
#include "GuiBuilder.hpp"
#include <Filesystem.hpp>

constexpr const char* SELECT_PACK_ID = "SelectPackId";
constexpr const char* SELECT_LEVEL_ID = "SelectLevelId";

LoadLevelDialog::LoadLevelDialog(
    mem::Rc<Gui> gui, const std::filesystem::path& rootDir)
    : DialogInterface(
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
    // must be initialized each time a modal is opened
    mapPackNames = Filesystem::getLevelPackNames(levelsDir);
    panel->setWidgetName(SELECT_LEVEL_ID + std::string("LayoutPanel"));
    panel->add(
        FormBuilder()
            .addOptionWithWidgetId(
                Strings::Dialog::Body::SELECT_PACK,
                WidgetBuilder::createDropdown(
                    mapPackNames,
                    mapPackName,
                    std::bind(
                        &LoadLevelDialog::handleSelectedMapPack,
                        this,
                        std::placeholders::_1)),
                SELECT_PACK_ID)
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

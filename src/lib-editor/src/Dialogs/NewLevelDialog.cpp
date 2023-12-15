#include "Dialogs/NewLevelDialog.hpp"
#include "Globals.hpp"
#include <Configs/Strings.hpp>

import GuiBuilder;

ModernNewLevelDialog::ModernNewLevelDialog(mem::Rc<Gui> gui)
    : ModernDialogInterface(
        gui, "ModernNewLevelDialog", Strings::Editor::NewLevel::TITLE)
{
}

void ModernNewLevelDialog::buildLayoutImpl(tgui::Panel::Ptr panel)
{
    FormBuilder(panel)
        .addOption(
            Strings::Editor::NewLevel::WIDTH,
            WidgetBuilder::createDropdown(
                ALLOWED_LEVEL_SIZES,
                std::to_string(width),
                [this](std::size_t idx)
                { width = std::stol(ALLOWED_LEVEL_SIZES[idx]); }))
        .addOption(
            Strings::Editor::NewLevel::HEIGHT,
            WidgetBuilder::createDropdown(
                ALLOWED_LEVEL_SIZES,
                std::to_string(height),
                [this](std::size_t idx)
                { height = std::stol(ALLOWED_LEVEL_SIZES[idx]); }))
        .addOption(
            Strings::Editor::NewLevel::THEME,
            WidgetBuilder::createDropdown(
                LevelThemeUtils::getAllNames(),
                LevelThemeUtils::toString(theme),
                [this](std::size_t idx)
                { theme = static_cast<LevelTheme>(idx); }))
        .build();
}

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
            Strings::Editor::NewLevel::SKYBOX_THEME,
            WidgetBuilder::createDropdown(
                SkyboxThemeUtils::getAllNames(),
                SkyboxThemeUtils::toString(skyboxTheme),
                [this](std::size_t idx)
                { skyboxTheme = static_cast<SkyboxTheme>(idx); }))
        .addOption(
            Strings::Editor::NewLevel::TEXTURE_PACK,
            WidgetBuilder::createDropdown(
                TexturePackUtils::getAllNames(),
                TexturePackUtils::toString(texturePack),
                [this](std::size_t idx)
                { skyboxTheme = static_cast<SkyboxTheme>(idx); }))
        .addOption(
            Strings::Editor::NewLevel::AUTHOR,
            WidgetBuilder::createTextInput(
                author,
                [this](tgui::String str) { author = str.toStdString(); }))
        .build();
}

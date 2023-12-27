#include "Dialogs/MetadataDialogBase.hpp"
#include <Configs/Strings.hpp>

import WidgetBuilder;

void MetadataDialogBase::addAttributesToFormular(FormBuilder& builder)
{
    std::ignore =
        builder
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
                    { texturePack = static_cast<TexturePack>(idx); }))
            .addOption(
                Strings::Editor::NewLevel::AUTHOR,
                WidgetBuilder::createTextInput(
                    author,
                    [this](tgui::String str) { author = str.toStdString(); }));
}

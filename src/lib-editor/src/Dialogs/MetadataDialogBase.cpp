#include "Dialogs/MetadataDialogBase.hpp"
#include "WidgetBuilder.hpp"
#include <Configs/Strings.hpp>

void MetadataDialogBase::addAttributesToFormular(FormBuilder& builder)
{
    auto skyboxNames = SkyboxThemeUtils::getAllNames();
    auto texturePackNames = TexturePackUtils::getAllNames();

    std::ignore =
        builder
            .addOption(
                Strings::Editor::NewLevel::SKYBOX_THEME,
                WidgetBuilder::createDropdown(
                    skyboxNames,
                    skyboxNames.at(std::to_underlying(skyboxTheme)),
                    [this](std::size_t idx)
                    { skyboxTheme = static_cast<SkyboxTheme>(idx); }))
            .addOption(
                Strings::Editor::NewLevel::TEXTURE_PACK,
                WidgetBuilder::createDropdown(
                    texturePackNames,
                    texturePackNames.at(std::to_underlying(texturePack)),
                    [this](std::size_t idx)
                    { texturePack = static_cast<TexturePack>(idx); }))
            .addOption(
                Strings::Editor::NewLevel::AUTHOR,
                WidgetBuilder::createTextInput(
                    author,
                    [this](tgui::String str) { author = str.toStdString(); }));
}

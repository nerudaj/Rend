#include "LevelTheme.hpp"
#include <Configs/Strings.hpp>
#include <cassert>

SkyboxTheme SkyboxThemeUtils::fromString(const std::string& str)
{
    if (str == Strings::Level::SKYBOX_DAWNTIME)
        return SkyboxTheme::Dawntime;
    else if (str == Strings::Level::SKYBOX_SPACE)
        return SkyboxTheme::Nightsky;

    // Default to countryside if there is anything wrong
    return SkyboxTheme::Countryside;
}

std::string SkyboxThemeUtils::toString(SkyboxTheme skyboxTheme)
{
    switch (skyboxTheme)
    {
        using enum SkyboxTheme;
    case Countryside:
        return Strings::Level::SKYBOX_COUNTRY;
    case Dawntime:
        return Strings::Level::SKYBOX_DAWNTIME;
    case Nightsky:
        return Strings::Level::SKYBOX_SPACE;
    }
}

std::vector<std::string> SkyboxThemeUtils::getAllNames()
{
    return std::vector<std::string> { toString(SkyboxTheme::Countryside),
                                      toString(SkyboxTheme::Dawntime),
                                      toString(SkyboxTheme::Nightsky) };
}

TexturePack TexturePackUtils::fromString(const std::string& str)
{
    if (str == Strings::Level::TEXTURES_SPACE)
        return TexturePack::SpaceStation;
    else if (str == Strings::Level::TEXTURES_COUNTRY)
        return TexturePack::CountrySide;
    else if (str == Strings::Level::TEXTURES_NEON)
        return TexturePack::Neon;

    // Default to alpha if there is anything wrong
    return TexturePack::AlphaVersion;
}

std::string TexturePackUtils::toString(TexturePack levelTheme)
{
    switch (levelTheme)
    {
        using enum TexturePack;
    case AlphaVersion:
        return Strings::Level::TEXTURES_ALPHA;
    case SpaceStation:
        return Strings::Level::TEXTURES_SPACE;
    case CountrySide:
        return Strings::Level::TEXTURES_COUNTRY;
    case Neon:
        return Strings::Level::TEXTURES_NEON;
    }
}

std::vector<std::string> TexturePackUtils::getAllNames()
{
    return std::vector<std::string> { toString(TexturePack::AlphaVersion),
                                      toString(TexturePack::SpaceStation),
                                      toString(TexturePack::CountrySide),
                                      toString(TexturePack::Neon) };
}

LevelTheme LevelTheme::fromJson(const std::string& str) noexcept
{
    if (str.empty()) return LevelTheme {};

    try
    {
        auto json = nlohmann::json::parse(str);
        LevelTheme theme = json;
        return theme;
    }
    catch (std::exception& e)
    {
        std::cerr << std::format(
            "Error parsing JSON string '{}' with reason: {}\n", str, e.what());
        return LevelTheme {};
    }
}
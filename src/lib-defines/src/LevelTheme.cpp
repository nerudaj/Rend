#include "LevelTheme.hpp"
#include <Configs/Strings.hpp>
#include <cassert>

SkyboxTheme SkyboxThemeUtils::fromString(const std::string& str)
{
    if (str == Strings::Level::THEME_DAWNTIME)
        return SkyboxTheme::Dawntime;
    else if (str == Strings::Level::THEME_SPACE)
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
        return Strings::Level::THEME_COUNTRY;
    case Dawntime:
        return Strings::Level::THEME_DAWNTIME;
    case Nightsky:
        return Strings::Level::THEME_SPACE;
    }
}

std::vector<std::string> SkyboxThemeUtils::getAllNames()
{
    return std::vector<std::string> { toString(SkyboxTheme::Countryside),
                                      toString(SkyboxTheme::Dawntime),
                                      toString(SkyboxTheme::Nightsky) };
}

TexturePack TexturePackUtils::fromString(const std::string&)
{
    // Default to alpha if there is anything wrong
    return TexturePack::AlphaVersion;
}

std::string TexturePackUtils::toString(TexturePack levelTheme)
{
    switch (levelTheme)
    {
        using enum TexturePack;
    case AlphaVersion:
        return Strings::Level::THEME_ALPHA;
    }
}

std::vector<std::string> TexturePackUtils::getAllNames()
{
    return std::vector<std::string> { toString(TexturePack::AlphaVersion) };
}

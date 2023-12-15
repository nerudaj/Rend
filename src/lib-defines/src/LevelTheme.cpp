#include "LevelTheme.hpp"
#include <Configs/Strings.hpp>
#include <cassert>

LevelTheme LevelThemeUtils::fromString(const std::string& str)
{
    if (str == Strings::Level::THEME_COUNTRY)
        return LevelTheme::Countryside;
    else if (str == Strings::Level::THEME_DAWNTIME)
        return LevelTheme::Dawntime;
    assert(str == Strings::Level::THEME_SPACE);
    return LevelTheme::SpaceStation;
}

std::string LevelThemeUtils::toString(LevelTheme theme)
{
    switch (theme)
    {
        using enum LevelTheme;
    case Countryside:
        return Strings::Level::THEME_COUNTRY;
    case Dawntime:
        return Strings::Level::THEME_DAWNTIME;
    case SpaceStation:
        return Strings::Level::THEME_SPACE;
    }
}

std::vector<std::string> LevelThemeUtils::getAllNames()
{
    return std::vector<std::string> { toString(LevelTheme::Countryside),
                                      toString(LevelTheme::Dawntime),
                                      toString(LevelTheme::SpaceStation) };
}

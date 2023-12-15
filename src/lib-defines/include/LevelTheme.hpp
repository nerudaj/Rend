#pragma once

#include <string>
#include <vector>

enum class [[nodiscard]] LevelTheme
{
    Countryside,
    Dawntime,
    SpaceStation
};

class LevelThemeUtils
{
    static LevelTheme fromString(const std::string& str);

    static [[nodiscard]] std::string toString(LevelTheme theme);

    static [[nodiscard]] std::vector<std::string> getAllNames();
};
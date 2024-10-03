#pragma once

#include <Configs/Strings.hpp>
#include <nlohmann/json.hpp>
#include <ranges>
#include <string>
#include <vector>

enum class [[nodiscard]] SkyboxTheme
{
    Countryside,
    Dawntime,
    Nightsky
};

NLOHMANN_JSON_SERIALIZE_ENUM(
    SkyboxTheme,
    {
        { SkyboxTheme::Countryside, Strings::Level::SKYBOX_COUNTRY },
        { SkyboxTheme::Dawntime, Strings::Level::SKYBOX_DAWNTIME },
        { SkyboxTheme::Nightsky, Strings::Level::SKYBOX_SPACE },
    });

class SkyboxThemeUtils
{
public:
    static [[nodiscard]] std::vector<std::string> getAllNames()
    {
        return { Strings::Level::SKYBOX_COUNTRY,
                 Strings::Level::SKYBOX_DAWNTIME,
                 Strings::Level::SKYBOX_SPACE };
    }
};

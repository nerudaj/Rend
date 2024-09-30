#pragma once

#include <Configs/Strings.hpp>
#include <nlohmann/json.hpp>
#include <ranges>
#include <string>
#include <vector>

enum class [[nodiscard]] TexturePack
{
    SpaceStation,
    CountrySide,
    Neon
};

NLOHMANN_JSON_SERIALIZE_ENUM(
    TexturePack,
    {
        { TexturePack::SpaceStation, Strings::Level::TEXTURES_SPACE },
        { TexturePack::CountrySide, Strings::Level::TEXTURES_COUNTRY },
        { TexturePack::Neon, Strings::Level::TEXTURES_NEON },
    });

class TexturePackUtils
{
public:
    static [[nodiscard]] std::vector<std::string> getAllNames()
    {
        return {
            Strings::Level::TEXTURES_SPACE,
            Strings::Level::TEXTURES_COUNTRY,
            Strings::Level::TEXTURES_NEON,
        };
    }
};

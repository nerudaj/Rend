#pragma once

#include <Configs/Strings.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

enum class [[nodiscard]] MapCompatibility
{
    Deathmatch,
    SingleFlagCTF,
};

NLOHMANN_JSON_SERIALIZE_ENUM(
    MapCompatibility,
    {
        { MapCompatibility::Deathmatch, Strings::Level::GAMEMODE_DM },
        { MapCompatibility::SingleFlagCTF, Strings::Level::GAMEMODE_SCTF },
    });

class MapCompatibilityUtils final
{
public:
    static [[nodiscard]] std::vector<std::string> getAllNames()
    {
        return { Strings::Level::GAMEMODE_DM, Strings::Level::GAMEMODE_SCTF };
    }
};

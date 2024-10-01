#pragma once

#include <nlohmann/json.hpp>

enum class [[nodiscard]] GameMode
{
    Deathmatch,
    SingleFlagCtf,
};

NLOHMANN_JSON_SERIALIZE_ENUM(
    GameMode,
    {
        { GameMode::Deathmatch, "dm" },
        { GameMode::SingleFlagCtf, "sctf" },
    });

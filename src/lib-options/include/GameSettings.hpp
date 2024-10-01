#pragma once

#include <nlohmann/json.hpp>
#include "enums/PlayerKind.hpp"
#include "enums/GameMode.hpp"
#include <string>
#include <vector>

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

struct PlayerOptions
{
    PlayerKind kind = PlayerKind::LocalHuman;
    bool bindCamera = false;
    std::string name = "";
    bool autoswapOnPickup = false;
};

struct GameOptions
{
    std::vector<PlayerOptions> players = { PlayerOptions {
        .kind = PlayerKind::LocalHuman, .bindCamera = true } };
    GameMode gameMode = GameMode::Deathmatch;
    unsigned pointlimit = 15;
};
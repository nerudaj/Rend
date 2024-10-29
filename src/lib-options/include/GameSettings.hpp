#pragma once

#include "PlayerOptions.hpp"
#include "enums/GameMode.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

struct GameOptions
{
    std::vector<PlayerOptions> players = { PlayerOptions {
        .kind = PlayerKind::LocalHuman, .bindCamera = true } };
    GameMode gameMode = GameMode::Deathmatch;
    unsigned pointlimit = 15;
};
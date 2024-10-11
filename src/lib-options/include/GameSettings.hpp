#pragma once

#include "enums/GameMode.hpp"
#include "enums/PlayerKind.hpp"
#include "enums/Team.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

struct PlayerOptions
{
    PlayerKind kind = PlayerKind::LocalHuman;
    bool bindCamera = false;
    std::string name = "";
    bool autoswapOnPickup = false;
    Team team = Team::None;
};

struct GameOptions
{
    std::vector<PlayerOptions> players = { PlayerOptions {
        .kind = PlayerKind::LocalHuman, .bindCamera = true } };
    GameMode gameMode = GameMode::Deathmatch;
    unsigned pointlimit = 15;
};
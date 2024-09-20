#pragma once

#include <string>
#include <vector>

enum class [[nodiscard]] PlayerKind
{
    LocalHuman,
    LocalNpc,
    RemoteHuman
};

enum class [[nodiscard]] GameMode
{
    Deathmatch,
    SingleFlagCtf,
};

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
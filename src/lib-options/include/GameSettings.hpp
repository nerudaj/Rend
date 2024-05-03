#pragma once

#include <string>
#include <vector>

enum class PlayerKind { LocalHuman, LocalNpc, RemoteHuman };

struct PlayerOptions
{
    PlayerKind kind = PlayerKind::LocalHuman;
    bool bindCamera = false;
    std::string name = "";
};

struct GameOptions
{
    std::vector<PlayerOptions> players = { PlayerOptions {
        .kind = PlayerKind::LocalHuman, .bindCamera = true } };
    unsigned fraglimit = 15;
};
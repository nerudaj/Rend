module;

#include <string>
#include <vector>

export module GameOptions;

export enum class PlayerKind { LocalHuman, LocalNpc, RemoteHuman };

export struct PlayerOptions
{
    PlayerKind kind = PlayerKind::LocalHuman;
    bool bindCamera = false;
    std::string name = "";
};

export struct GameOptions
{
    std::vector<PlayerOptions> players = { PlayerOptions {
        .kind = PlayerKind::LocalHuman, .bindCamera = true } };
    unsigned fraglimit = 15;
};
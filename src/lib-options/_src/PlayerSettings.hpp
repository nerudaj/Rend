#pragma once

enum class PlayerKind
{
    LocalHuman,
    LocalNpc,
    RemoteHuman
};

struct PlayerSettings
{
    PlayerKind kind;
    bool bindCamera;
};

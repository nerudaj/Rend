#pragma once

#include "enums/PlayerKind.hpp"
#include "enums/Team.hpp"
#include <string>

struct PlayerOptions
{
    PlayerKind kind = PlayerKind::LocalHuman;
    bool bindCamera = false;
    std::string name = "";
    bool autoswapOnPickup = false;
    Team team = Team::None;
};

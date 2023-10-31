#pragma once

#include "PlayerSettings.hpp"
#include <string>
#include <vector>

struct GameSettings
{
    std::string map;
    std::vector<PlayerSettings> players;
    int fraglimit;
};

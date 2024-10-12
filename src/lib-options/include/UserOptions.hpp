#pragma once

#include "enums/Team.hpp"
#include <nlohmann/json.hpp>
#include <string>

struct [[nodiscard]] UserOptions final
{
    std::string name = "player";
    bool autoswapOnPickup = true;
    Team prefferedTeam = Team::None;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    UserOptions, name, autoswapOnPickup, prefferedTeam);

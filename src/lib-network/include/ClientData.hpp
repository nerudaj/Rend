#pragma once

#include <nlohmann/json.hpp>
#include <string>

struct [[nodiscard]] ClientData final
{
    bool active = true;
    std::string name = "player";
    bool isReady = true;
    bool hasAutoswapOnPickup = false;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    ClientData, active, name, isReady, hasAutoswapOnPickup);

#pragma once

#include <nlohmann/json.hpp>
#include <string>

struct [[nodiscard]] UserOptions final
{
    std::string name = "player";
    bool autoswapOnPickup = true;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UserOptions, name, autoswapOnPickup);

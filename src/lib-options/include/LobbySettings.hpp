#pragma once

#include "MapOptions.hpp"
#include <enums/GameMode.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

struct [[nodiscard]] LobbySettings final
{
    std::string packname = "";
    std::vector<MapOptions> mapSettings = {};
    std::vector<size_t> mapOrder = {};
    bool useRandomMapRotation = true;
    GameMode gameMode = GameMode::Deathmatch;
    int pointlimit = 15;
    size_t maxNpcs = 3;

    [[nodiscard]] constexpr std::partial_ordering
    operator<=>(const LobbySettings&) const = default;

    [[nodiscard]] constexpr bool isEmpty() const
    {
        return *this == LobbySettings {};
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    LobbySettings,
    packname,
    mapSettings,
    mapOrder,
    useRandomMapRotation,
    gameMode,
    pointlimit,
    maxNpcs);

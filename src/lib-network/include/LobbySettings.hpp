#pragma once

#include <nlohmann/json.hpp>
#include <string>

struct [[nodiscard]] MapSettings final
{
    std::string name = "";
    bool enabled = false;

    [[nodiscard]] constexpr std::partial_ordering
    operator<=>(const MapSettings&) const = default;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapSettings, name, enabled);

struct [[nodiscard]] LobbySettings final
{
    std::string packname = "";
    std::vector<MapSettings> mapSettings = {};
    std::vector<size_t> mapOrder = {};
    int pointlimit = 0;
    size_t maxNpcs = 0;

    [[nodiscard]] constexpr std::partial_ordering
    operator<=>(const LobbySettings&) const = default;

    [[nodiscard]] constexpr bool isEmpty() const
    {
        return *this == LobbySettings {};
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    LobbySettings, packname, mapSettings, mapOrder, pointlimit, maxNpcs);

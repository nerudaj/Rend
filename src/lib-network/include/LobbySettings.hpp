#pragma once

#include <nlohmann/json.hpp>
#include <string>

struct [[nodiscard]] MapSettings final
{
    std::string name;
    bool enabled;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapSettings, name, enabled);

struct [[nodiscard]] LobbySettings final
{
    std::string packname;
    std::vector<MapSettings> mapSettings;
    std::vector<size_t> mapOrder;
    int fraglimit;
    unsigned maxNpcs;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    LobbySettings, packname, mapSettings, mapOrder, fraglimit, maxNpcs);

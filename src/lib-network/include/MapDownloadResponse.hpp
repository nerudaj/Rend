#pragma once

#include <nlohmann/json.hpp>

struct [[nodiscard]] MapDownloadResponse final
{
    std::string mapPackName;
    std::string mapName;
    std::string base64encodedMap;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    MapDownloadResponse, mapPackName, mapName, base64encodedMap);

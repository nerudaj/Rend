#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

struct [[nodiscard]] MapDownloadRequest final
{
    std::string mapPackName;
    std::vector<std::string> mapNames;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapDownloadRequest, mapPackName, mapNames);

#pragma once

#include "enums/MapCompatibility.hpp"
#include "enums/SkyboxTheme.hpp"
#include "enums/TexturePack.hpp"
#include <format>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

// Version before adding MapCompatibility, required for older maps
struct [[nodiscard]] LegacyMapProperties final
{
    SkyboxTheme skybox;
    TexturePack textures;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LegacyMapProperties, skybox, textures);

struct [[nodiscard]] MapProperties final
{
    SkyboxTheme skyboxTheme;
    TexturePack texturePack;
    MapCompatibility mapCompat;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    MapProperties, skyboxTheme, texturePack, mapCompat);

static MapProperties parseMapProperties(const std::string& str)
{
    try
    {
        MapProperties props = nlohmann::json::parse(str);
        return props;
    }
    catch (...)
    {
        LegacyMapProperties props = nlohmann::json::parse(str);
        return MapProperties {
            .skyboxTheme = props.skybox,
            .texturePack = props.textures,
            .mapCompat = MapCompatibility::Deathmatch,
        };
    }
}

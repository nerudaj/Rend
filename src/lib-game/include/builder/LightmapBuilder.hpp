#pragma once

#include <DGM/classes/Objects.hpp>
#include <LevelD.hpp>
#include <LevelTheme.hpp>
#include <SFML/System/Vector2.hpp>
#include <core/Scene.hpp>
#include <queue>
#include <type_traits>

struct LightPoint
{
    MeshItrType x, y;
    LightType lightLevel;
    LightType decayAmount;
    // drop this light source if bottom tile is blocking
    bool skipOnBottomSolid;
};

class LightmapBuilder final
{
public:
    static LightmapType
    buildLightmap(const LevelD& level, SkyboxTheme skyboxTheme);

private:
    static std::queue<LightPoint> getTileBasedLightSources(
        const LevelD::TileLayer& layer,
        const sf::Vector2u layerSize,
        SkyboxTheme skybox,
        bool skipOnBottomSolid = false);

    static std::queue<LightPoint> getItemBasedLightSources(
        const LevelD& level, const sf::Vector2u& voxelSize);

    static void processLightPointQueue(
        std::queue<LightPoint> queue,
        std::vector<LightType>& data,
        const sf::Vector2u& dataSize,
        const std::vector<bool>& blockingTiles);

    static void mergeLightmaps(
        const std::vector<LightType>& source,
        std::vector<LightType>& destination);
};

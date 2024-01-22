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
};

class LightmapBuilder final
{
public:
    static LightmapType
    buildLightmap(const LevelD& level, SkyboxTheme skyboxTheme);

private:
    static std::queue<LightPoint> getVerticalLightSources(
        const LevelD::TileLayer& layer,
        const sf::Vector2u& layerSize,
        SkyboxTheme skybox);

    static std::queue<LightPoint> getHorizontalLightSources(
        const LevelD::TileLayer& layer,
        const sf::Vector2u layerSize,
        SkyboxTheme skybox);

    static std::queue<LightPoint> getItemBasedLightSources(
        const LevelD& level, const sf::Vector2u& voxelSize);

    static void processLightPointQueue(
        std::queue<LightPoint> queue,
        std::vector<LightType>& data,
        const sf::Vector2u& dataSize,
        const std::vector<bool>& blockingTiles);
};

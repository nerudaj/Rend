#pragma once

#include <DGM/classes/Objects.hpp>
#include <LevelD.hpp>
#include <SFML/System/Vector2.hpp>
#include <core/Types.hpp>
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
    static LightmapType buildLightmap(
        const dgm::Mesh& bottom, const dgm::Mesh& upper, const LevelD& level);

private:
    static std::queue<LightPoint>
    getTileBasedLightSources(const dgm::Mesh& bottom, const dgm::Mesh& upper);

    static std::queue<LightPoint> getItemBasedLightSources(
        const LevelD& level, const sf::Vector2u& voxelSize);

    static void processLightPointQueue(
        std::queue<LightPoint> queue,
        std::vector<LightType>& data,
        const dgm::Mesh& bottom,
        const dgm::Mesh& upper);
};

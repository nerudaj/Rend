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
};

class LightmapBuilder final
{
public:
    static LightmapType buildLightmap(
        const dgm::Mesh& bottom, const dgm::Mesh& upper, const LevelD& level);

private:
    static std::vector<LightType>
    getDataWithBaseLightLevels(const dgm::Mesh& bottom, const dgm::Mesh& upper);

    static std::queue<LightPoint> getInitialLightSourceQueue(
        const LevelD& level, const sf::Vector2u& voxelSize);
};

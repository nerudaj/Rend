#include <core/Enums.hpp>
#include <utils/LightmapBuilder.hpp>

constexpr LightType OUTSIDE_DAY_LIGHT_LEVEL = 224;
constexpr LightType INSIDE_CORRIDOR_LIGHT_LEVEL = 96;
constexpr LightType INSIDE_ROOM_LIGHT_LEVEL = 192; // Currently unused
constexpr LightType ARTIFICIAL_LIGHT_SOURCE_LEVEL = 255;
constexpr LightType LIGHT_DECAY_AMOUNT = 16;

LightmapType LightmapBuilder::buildLightmap(
    const dgm::Mesh& bottom, const dgm::Mesh& upper, const LevelD& level)
{
    const auto& voxelSize = upper.getVoxelSize();
    auto data = getDataWithBaseLightLevels(bottom, upper);
    auto lightPointsToFloodFrom = getInitialLightSourceQueue(level, voxelSize);

    while (!lightPointsToFloodFrom.empty())
    {
        auto front = lightPointsToFloodFrom.front();
        lightPointsToFloodFrom.pop();

        const auto index = front.y * voxelSize.x + front.x;

        auto currentLevel = data.at(index);
        if (0 == currentLevel || currentLevel >= front.lightLevel) continue;

        data.at(index) = front.lightLevel;
        const auto decayedLight =
            static_cast<LightType>(front.lightLevel - LIGHT_DECAY_AMOUNT);
        lightPointsToFloodFrom.push(LightPoint {
            .x = front.x - 1, .y = front.y, .lightLevel = decayedLight });
        lightPointsToFloodFrom.push(LightPoint {
            .x = front.x + 1, .y = front.y, .lightLevel = decayedLight });
        lightPointsToFloodFrom.push(LightPoint {
            .x = front.x, .y = front.y - 1, .lightLevel = decayedLight });
        lightPointsToFloodFrom.push(LightPoint {
            .x = front.x, .y = front.y + 1, .lightLevel = decayedLight });
    }

    return LightmapType(data, bottom.getDataSize(), voxelSize);
}

std::vector<LightType> LightmapBuilder::getDataWithBaseLightLevels(
    const dgm::Mesh& bottom, const dgm::Mesh& upper)
{
    const auto dataSize = bottom.getDataSize().x * bottom.getDataSize().y;
    auto data = std::vector<LightType>(dataSize, 0);

    // Set base levels
    for (MeshItrType y = 0, i = 0; y < bottom.getDataSize().y; ++y)
    {
        for (MeshItrType x = 0; x < bottom.getDataSize().y; ++x, ++i)
        {
            if (static_cast<TilesetMapping>(upper.at(x, y))
                == TilesetMapping::CeilSky)
            {
                data[i] = OUTSIDE_DAY_LIGHT_LEVEL;
            }
            else if (
                static_cast<TilesetMapping>(bottom.at(x, y))
                > TilesetMapping::CeilLow)
            {
                data[i] = 0; // inside wall
            }
            else
            {
                data[i] = INSIDE_CORRIDOR_LIGHT_LEVEL;
            }
        }
    }

    return data;
}

std::queue<LightPoint> LightmapBuilder::getInitialLightSourceQueue(
    const LevelD& level, const sf::Vector2u& voxelSize)
{
    std::queue<LightPoint> queue;

    for (auto&& thing : level.things)
    {
        using enum LevelItemId;
        switch (static_cast<LevelItemId>(thing.id))
        {
        case FloorLamp:
            queue.push(
                LightPoint { .x = thing.x / voxelSize.x,
                             .y = thing.y / voxelSize.y,
                             .lightLevel = ARTIFICIAL_LIGHT_SOURCE_LEVEL });
            break;
        default:
            break;
        }
    }

    return queue;
}

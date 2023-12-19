#include <builder/LightmapBuilder.hpp>
#include <core/Enums.hpp>

constexpr LightType NATURAL_OUTDOOR_LIGHT_LEVEL = 255;
constexpr LightType ARTIFICIAL_LIGHT_LEVEL = 192;
constexpr LightType VERTICAL_LIGHT_DECAY_AMOUNT =
    64; // anything that comes from tiles
constexpr LightType HORIZONTAL_LIGHT_DECAY_AMOUNT =
    32; // anything that comes from items

LightmapType LightmapBuilder::buildLightmap(
    const dgm::Mesh& bottom, const dgm::Mesh& upper, const LevelD& level)
{
    const auto& voxelSize = upper.getVoxelSize();
    const auto dataSize = bottom.getDataSize().x * bottom.getDataSize().y;
    auto data = std::vector<LightType>(dataSize, 0);

    processLightPointQueue(
        getTileBasedLightSources(bottom, upper), data, bottom, upper);
    processLightPointQueue(
        getItemBasedLightSources(level, voxelSize), data, bottom, upper);

    return LightmapType(data, bottom.getDataSize(), voxelSize);
}

std::queue<LightPoint> LightmapBuilder::getTileBasedLightSources(
    const dgm::Mesh& bottom, const dgm::Mesh& upper)
{
    std::queue<LightPoint> queue;

    // Set base levels
    for (MeshItrType y = 0, i = 0; y < bottom.getDataSize().y; ++y)
    {
        for (MeshItrType x = 0; x < bottom.getDataSize().y; ++x, ++i)
        {
            auto upperTileType = static_cast<LevelTileId>(upper.at(x, y));
            auto bottomTileType = static_cast<LevelTileId>(bottom.at(x, y));

            if (upperTileType == LevelTileId::CeilSky)
            {
                queue.emplace(
                    LightPoint { .x = x,
                                 .y = y,
                                 .lightLevel = NATURAL_OUTDOOR_LIGHT_LEVEL,
                                 .decayAmount = VERTICAL_LIGHT_DECAY_AMOUNT });
            }
            else if (
                bottomTileType == LevelTileId::FlatLight
                || upperTileType == LevelTileId::FlatLight)
            {
                queue.emplace(
                    LightPoint { .x = x,
                                 .y = y,
                                 .lightLevel = ARTIFICIAL_LIGHT_LEVEL,
                                 .decayAmount = VERTICAL_LIGHT_DECAY_AMOUNT });
            }
        }
    }

    return queue;
}

std::queue<LightPoint> LightmapBuilder::getItemBasedLightSources(
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
                             .lightLevel = ARTIFICIAL_LIGHT_LEVEL,
                             .decayAmount = HORIZONTAL_LIGHT_DECAY_AMOUNT });
            break;
        default:
            break;
        }
    }

    return queue;
}

void LightmapBuilder::processLightPointQueue(
    std::queue<LightPoint> queue,
    std::vector<LightType>& data,
    const dgm::Mesh& bottom,
    const dgm::Mesh& upper)
{

    while (!queue.empty())
    {
        auto front = queue.front();
        queue.pop();

        if (front.y >= bottom.getDataSize().y
            || front.x >= bottom.getDataSize().x)
            continue;

        const auto index = front.y * bottom.getDataSize().x + front.x;

        auto currentLevel = data.at(index);
        if ((bottom[index] > 4 && upper[index] > 4)
            || currentLevel > front.lightLevel)
            continue;

        data.at(index) = front.lightLevel;
        if (front.lightLevel < front.decayAmount) continue;

        const auto decayedLight =
            static_cast<LightType>(front.lightLevel - front.decayAmount);
        queue.push(LightPoint { .x = front.x - 1,
                                .y = front.y,
                                .lightLevel = decayedLight,
                                .decayAmount = front.decayAmount });
        queue.push(LightPoint { .x = front.x + 1,
                                .y = front.y,
                                .lightLevel = decayedLight,
                                .decayAmount = front.decayAmount });
        queue.push(LightPoint { .x = front.x,
                                .y = front.y - 1,
                                .lightLevel = decayedLight,
                                .decayAmount = front.decayAmount });
        queue.push(LightPoint { .x = front.x,
                                .y = front.y + 1,
                                .lightLevel = decayedLight,
                                .decayAmount = front.decayAmount });
    }
}

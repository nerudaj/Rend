#include <builder/LightmapBuilder.hpp>
#include <core/Enums.hpp>
#include <utils/SemanticTypes.hpp>

constexpr LightType NATURAL_OUTDOOR_LIGHT_LEVEL = 255;
constexpr LightType DIMMED_NATURAL_OUTDOOR_LIGHT_LEVEL = 128;
constexpr LightType NIGHT_NATURAL_OUTDOOR_LIGHT_LEVEL = 32;
constexpr LightType ARTIFICIAL_LIGHT_LEVEL = 192;
constexpr LightType VERTICAL_LIGHT_DECAY_AMOUNT =
    64; // anything that comes from tiles
constexpr LightType HORIZONTAL_LIGHT_DECAY_AMOUNT =
    32; // anything that comes from items

LightmapType
LightmapBuilder::buildLightmap(const LevelD& level, SkyboxTheme skyboxTheme)
{
    const auto& voxelSize =
        sf::Vector2u(level.mesh.tileWidth, level.mesh.tileHeight);
    const auto dataSize =
        sf::Vector2u(level.mesh.layerWidth, level.mesh.layerHeight);
    auto data = std::vector<LightType>(dataSize.x * dataSize.y, 0);

    processLightPointQueue(
        getTileBasedLightSources(
            level.mesh.layers.at(0),
            dataSize,
            skyboxTheme,
            "skipOnBottomSolid"_true),
        data,
        dataSize,
        level.mesh.layers.at(0).blocks);
    processLightPointQueue(
        getItemBasedLightSources(level, voxelSize),
        data,
        dataSize,
        level.mesh.layers.at(0).blocks);

    processLightPointQueue(
        getTileBasedLightSources(
            level.mesh.layers.at(1), dataSize, skyboxTheme),
        data,
        dataSize,
        level.mesh.layers.at(1).blocks);

    return LightmapType(data, dataSize, voxelSize);
}

[[nodiscard]] static constexpr bool isNaturalLightSource(LevelTileId id)
{
    using enum LevelTileId;
    return id == CeilSky || id == WallWindow;
}

[[nodiscard]] static constexpr bool isArtificialLightSource(LevelTileId id)
{
    using enum LevelTileId;
    return id == FlatLight || id == WallLight;
}

[[nodiscard]] static constexpr bool isLightSource(LevelTileId id)
{
    return isNaturalLightSource(id) || isArtificialLightSource(id);
}

[[nodiscard]] static constexpr LightType
getLightIntensity(LevelTileId tile, SkyboxTheme skybox)
{
    using enum SkyboxTheme;

    if (isNaturalLightSource(tile))
    {
        switch (skybox)
        {
        case Countryside:
            return NATURAL_OUTDOOR_LIGHT_LEVEL;
        case Dawntime:
            return DIMMED_NATURAL_OUTDOOR_LIGHT_LEVEL;
        case Nightsky:
            return NIGHT_NATURAL_OUTDOOR_LIGHT_LEVEL;
        }
    }
    else
    {
        return ARTIFICIAL_LIGHT_LEVEL;
    }
}

std::queue<LightPoint> LightmapBuilder::getTileBasedLightSources(
    const LevelD::TileLayer& layer,
    const sf::Vector2u layerSize,
    SkyboxTheme skybox,
    bool skipOnBottomSolid)
{
    std::queue<LightPoint> queue;

    for (MeshItrType y = 0, i = 0; y < layerSize.y; ++y)
    {
        for (MeshItrType x = 0; x < layerSize.x; ++x, ++i)
        {
            const bool isBlocking = layer.blocks.at(i);
            const auto tile = static_cast<LevelTileId>(layer.tiles.at(i));

            if (!isLightSource(tile)) continue;

            const auto lightAmount = getLightIntensity(tile, skybox);

            if (isBlocking)
            { // wall source
                queue.emplace(
                    x - 1,
                    y - 1,
                    lightAmount,
                    HORIZONTAL_LIGHT_DECAY_AMOUNT,
                    skipOnBottomSolid);
                queue.emplace(
                    x + 1,
                    y - 1,
                    lightAmount,
                    HORIZONTAL_LIGHT_DECAY_AMOUNT,
                    skipOnBottomSolid);
                queue.emplace(
                    x - 1,
                    y + 1,
                    lightAmount,
                    HORIZONTAL_LIGHT_DECAY_AMOUNT,
                    skipOnBottomSolid);
                queue.emplace(
                    x + 1,
                    y + 1,
                    lightAmount,
                    HORIZONTAL_LIGHT_DECAY_AMOUNT,
                    skipOnBottomSolid);
            }
            else // floor/ceil source
            {
                queue.emplace(x, y, lightAmount, VERTICAL_LIGHT_DECAY_AMOUNT);
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
        case CeilLamp:
            queue.emplace(
                thing.x / voxelSize.x,
                thing.y / voxelSize.y,
                ARTIFICIAL_LIGHT_LEVEL,
                HORIZONTAL_LIGHT_DECAY_AMOUNT,
                "skipOnBottomSolid"_true);
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
    const sf::Vector2u& dataSize,
    const std::vector<bool>& blockingTiles)
{
    while (!queue.empty())
    {
        auto front = queue.front();
        queue.pop();

        // Skip out-of-bounds tiles
        if (front.y >= dataSize.y || front.x >= dataSize.x) continue;

        const auto index = front.y * dataSize.x + front.x;
        if (blockingTiles.at(index) && front.skipOnBottomSolid) continue;

        const auto currentLightLevel = data.at(index);
        data.at(index) = std::max(currentLightLevel, front.lightLevel);

        if (front.lightLevel <= front.decayAmount) continue;

        const LightType decayedLight =
            front.lightLevel == 255 ? 224
                                    : front.lightLevel - front.decayAmount;
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

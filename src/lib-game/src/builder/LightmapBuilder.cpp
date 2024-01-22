#include <builder/LightmapBuilder.hpp>
#include <core/Enums.hpp>
#include <ranges>
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
    auto bottomData = std::vector<LightType>(
        dataSize.x * dataSize.y, NIGHT_NATURAL_OUTDOOR_LIGHT_LEVEL);
    auto upperData = std::vector<LightType>(
        dataSize.x * dataSize.y, NIGHT_NATURAL_OUTDOOR_LIGHT_LEVEL);

    auto bottomVertical =
        getVerticalLightSources(level.mesh.layers.at(0), dataSize, skyboxTheme);
    auto upperVertical =
        getVerticalLightSources(level.mesh.layers.at(1), dataSize, skyboxTheme);

    // 1) get vertical light sources VL
    // 2) get item-based light sources IL
    // 3) get bottom horizontal light sources BHL
    // 4) compute bottom data Bd from VL, IL and BHL
    processLightPointQueue(
        bottomVertical, bottomData, dataSize, level.mesh.layers.at(0).blocks);
    processLightPointQueue(
        upperVertical, bottomData, dataSize, level.mesh.layers.at(0).blocks);
    processLightPointQueue(
        getHorizontalLightSources(
            level.mesh.layers.at(0), dataSize, skyboxTheme),
        bottomData,
        dataSize,
        level.mesh.layers.at(0).blocks);
    processLightPointQueue(
        getItemBasedLightSources(level, voxelSize),
        bottomData,
        dataSize,
        level.mesh.layers.at(0).blocks);

    // 5) get upper horizontal light sources UHL
    // 6) compute upper data Ud from VL, UHL
    processLightPointQueue(
        bottomVertical, upperData, dataSize, level.mesh.layers.at(1).blocks);
    processLightPointQueue(
        upperVertical, upperData, dataSize, level.mesh.layers.at(1).blocks);
    processLightPointQueue(
        getHorizontalLightSources(
            level.mesh.layers.at(1), dataSize, skyboxTheme),
        upperData,
        dataSize,
        level.mesh.layers.at(1).blocks);

    // 7) compute result by merging Bd and Ud
    for (auto&& idx : std::views::iota(0u, bottomData.size()))
    {
        bottomData[idx] = std::max(bottomData[idx], upperData[idx]);
    }

    return LightmapType(bottomData, dataSize, voxelSize);
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

    return ARTIFICIAL_LIGHT_LEVEL;
}

static void performFloodStep(
    std::queue<LightPoint>& queue,
    MeshItrType x,
    MeshItrType y,
    LightType lightAmount,
    LightType decayAmount)
{
    queue.emplace(x - 1, y, lightAmount, decayAmount);
    queue.emplace(x + 1, y, lightAmount, decayAmount);
    queue.emplace(x, y - 1, lightAmount, decayAmount);
    queue.emplace(x, y + 1, lightAmount, decayAmount);
}

static void foreachTile(
    const sf::Vector2u& layerSize,
    std::function<void(MeshItrType x, MeshItrType y, MeshItrType index)>
        callback)
{
    for (MeshItrType y = 0, i = 0; y < layerSize.y; ++y)
    {
        for (MeshItrType x = 0; x < layerSize.x; ++x, ++i)
        {
            callback(x, y, i);
        }
    }
}

std::queue<LightPoint> LightmapBuilder::getVerticalLightSources(
    const LevelD::TileLayer& layer,
    const sf::Vector2u& layerSize,
    SkyboxTheme skybox)
{
    auto&& queue = std::queue<LightPoint> {};
    auto processTile = [&](MeshItrType x, MeshItrType y, MeshItrType idx)
    {
        if (layer.blocks.at(idx)) return;
        const auto tile = static_cast<LevelTileId>(layer.tiles.at(idx));

        if (isLightSource(tile))
        {
            const auto lightAmount = getLightIntensity(tile, skybox);
            queue.emplace(x, y, lightAmount, VERTICAL_LIGHT_DECAY_AMOUNT);
        }
    };

    foreachTile(layerSize, processTile);
    return queue;
}

std::queue<LightPoint> LightmapBuilder::getHorizontalLightSources(
    const LevelD::TileLayer& layer,
    const sf::Vector2u layerSize,
    SkyboxTheme skybox)
{
    std::queue<LightPoint> queue;
    auto processTile = [&](MeshItrType x, MeshItrType y, MeshItrType idx)
    {
        const auto tile = static_cast<LevelTileId>(layer.tiles.at(idx));

        if (layer.blocks.at(idx) && isLightSource(tile))
        {
            const auto lightAmount = getLightIntensity(tile, skybox);
            performFloodStep(
                queue, x, y, lightAmount, HORIZONTAL_LIGHT_DECAY_AMOUNT);
        }
    };

    foreachTile(layerSize, processTile);
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
                HORIZONTAL_LIGHT_DECAY_AMOUNT);
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
        if (blockingTiles.at(index)) continue;

        const auto currentLightLevel = data.at(index);
        data.at(index) = std::max(currentLightLevel, front.lightLevel);

        if (front.lightLevel <= front.decayAmount) continue;

        const LightType decayedLight = front.lightLevel - front.decayAmount
                                       + (front.lightLevel == 255 ? 1 : 0);
        performFloodStep(
            queue, front.x, front.y, decayedLight, front.decayAmount);
    }
}

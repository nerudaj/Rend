#include "render/RenderContext.hpp"
#include "utils/Builder.hpp"
#include <LevelD.hpp>
#include <core/Enums.hpp>
#include <queue>

[[nodiscard]] static sf::Text createTextObject(
    const sf::Font& font,
    unsigned charSize,
    dgm::UniversalReference<sf::Color> auto&& color)
{
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(charSize);
    text.setFillColor(std::forward<decltype(color)>(color));
    return text;
};

[[nodiscard]] static dgm::Mesh computeLightmap(
    const dgm::Mesh& bottom, const dgm::Mesh& upper, const LevelD& level)
{
    // TODO: change after mesh is template
    using LightType = std::remove_cvref<decltype(upper[0])>::type;

    LightType OUTSIDE_DAY_LIGHT_LEVEL = 224;
    LightType INSIDE_CORRIDOR_LIGHT_LEVEL = 96;
    LightType INSIDE_ROOM_LIGHT_LEVEL = 192; // Currently unused
    LightType ARTIFICIAL_LIGHT_SOURCE_LEVEL = 255;
    LightType LIGHT_DECAY_AMOUNT = 16;

    auto lightmap = dgm::Mesh();
    lightmap.setDataSize(bottom.getDataSize());
    lightmap.setVoxelSize(bottom.getVoxelSize());

    const auto& voxelSize = lightmap.getVoxelSize();

    using ItrType =
        decltype(std::remove_cvref<decltype(lightmap.getDataSize())>::type::x);

    // Set base levels
    for (ItrType y = 0; y < voxelSize.y; ++y)
    {
        for (ItrType x = 0; x < voxelSize.x; ++x)
        {
            if (static_cast<TilesetMapping>(upper.at(x, y))
                == TilesetMapping::CeilSky)
            {
                lightmap.at(x, y) = OUTSIDE_DAY_LIGHT_LEVEL;
            }
            else if (
                static_cast<TilesetMapping>(bottom.at(x, y))
                > TilesetMapping::CeilLow)
            {
                lightmap.at(x, y) = 0; // inside wall
            }
            else
            {
                lightmap.at(x, y) = INSIDE_CORRIDOR_LIGHT_LEVEL;
            }
        }
    }

    struct LightPoint
    {
        ItrType x, y;
        LightType lightLevel;
    };

    std::queue<LightPoint> lightPointsToFloodFrom;

    for (auto&& thing : level.things)
    {
        using enum LeveldItemId;
        switch (static_cast<LeveldItemId>(thing.id))
        {
        case FloorLamp:
            lightPointsToFloodFrom.push(
                LightPoint { .x = thing.x / voxelSize.x,
                             .y = thing.y / voxelSize.y,
                             .lightLevel = ARTIFICIAL_LIGHT_SOURCE_LEVEL });
            break;
        default:
            break;
        }
    }

    while (!lightPointsToFloodFrom.empty())
    {
        auto front = lightPointsToFloodFrom.front();
        lightPointsToFloodFrom.pop();

        auto currentLevel = lightmap.at(front.x, front.y);
        if (0 == currentLevel || currentLevel >= front.lightLevel) continue;

        lightmap.at(front.x, front.y) = front.lightLevel;
        lightPointsToFloodFrom.push(
            LightPoint { .x = front.x - 1,
                         .y = front.y,
                         .lightLevel = front.lightLevel - LIGHT_DECAY_AMOUNT });
        lightPointsToFloodFrom.push(
            LightPoint { .x = front.x + 1,
                         .y = front.y,
                         .lightLevel = front.lightLevel - LIGHT_DECAY_AMOUNT });
        lightPointsToFloodFrom.push(
            LightPoint { .x = front.x,
                         .y = front.y - 1,
                         .lightLevel = front.lightLevel - LIGHT_DECAY_AMOUNT });
        lightPointsToFloodFrom.push(
            LightPoint { .x = front.x,
                         .y = front.y + 1,
                         .lightLevel = front.lightLevel - LIGHT_DECAY_AMOUNT });
    }

    return lightmap;
}

[[nodiscard]] static DrawableLevel buildLevelRepresentation(const LevelD& level)
{
    auto bottomMesh = Builder::buildTextureMeshFromLvd(level, 0);
    auto upperMesh = Builder::buildTextureMeshFromLvd(level, 1);
    return DrawableLevel { .bottomTextures = bottomMesh,
                           .upperTextures = upperMesh,
                           .lightmap =
                               computeLightmap(bottomMesh, upperMesh, level) };
}

RenderContext RenderContext::buildRenderContext(
    const dgm::ResourceManager& resmgr,
    const std::string& mapname,
    unsigned screenWidth,
    unsigned screenHeight)
{
    auto&& tileset = resmgr.get<sf::Texture>("tileset.png").value().get();
    auto&& spritesheet = resmgr.get<sf::Texture>("items.png").value().get();
    auto&& hud = resmgr.get<sf::Texture>("weapons.png").value().get();
    auto&& tilesetClip =
        resmgr.get<dgm::Clip>("tileset.png.clip").value().get();
    auto&& spritesheetClip =
        resmgr.get<dgm::Clip>("items.png.clip").value().get();
    auto&& hudClipping =
        resmgr.get<dgm::Clip>("weapons.png.clip").value().get();
    auto&& shader = resmgr.get<sf::Shader>("shader").value().get();

    sf::RectangleShape weaponSprite;
    weaponSprite.setTexture(&hud);
    const float size = screenWidth / 3.f;
    weaponSprite.setSize({ size, size });
    weaponSprite.setPosition(
        { screenWidth / 2.f - size / 2.f, screenHeight - size });

    return RenderContext {
        .text = createTextObject(
            resmgr.get<sf::Font>("pico-8.ttf").value(), 32u, sf::Color::White),
        .level = buildLevelRepresentation(resmgr.get<LevelD>(mapname).value()),
        .tilesetTexture = tileset,
        .spritesheetTexture = spritesheet,
        .weaponHudTexture = hud,
        .tilesetClipping = tilesetClip,
        .spritesheetClipping = spritesheetClip,
        .weaponHudClipping = hudClipping,
        .shader = shader,
        .depthBuffer = std::vector<float>(screenWidth),
        .weaponSprite = weaponSprite
    };
}
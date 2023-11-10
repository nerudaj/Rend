#include "render/RenderContext.hpp"
#include "utils/Builder.hpp"
#include <LevelD.hpp>
#include <core/Enums.hpp>
#include <queue>
#include <utils/LightmapBuilder.hpp>

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

[[nodiscard]] static DrawableLevel buildLevelRepresentation(const LevelD& level)
{
    auto bottomMesh = Builder::buildTextureMeshFromLvd(level, 0);
    auto upperMesh = Builder::buildTextureMeshFromLvd(level, 1);
    return DrawableLevel { .bottomTextures = bottomMesh,
                           .upperTextures = upperMesh,
                           .lightmap = LightmapBuilder::buildLightmap(
                               bottomMesh, upperMesh, level) };
}

RenderContext RenderContext::buildRenderContext(
    const dgm::ResourceManager& resmgr,
    const std::string& mapname,
    unsigned screenWidth,
    unsigned screenHeight)
{
    const auto&& tileset = resmgr.get<sf::Texture>("tileset.png").value();
    const auto&& spritesheet = resmgr.get<sf::Texture>("items.png").value();
    const auto&& hud = resmgr.get<sf::Texture>("weapons.png").value();
    const auto&& tilesetClip =
        resmgr.get<dgm::Clip>("tileset.png.clip").value();
    const auto&& spritesheetClip =
        resmgr.get<dgm::Clip>("items.png.clip").value();
    const auto&& hudClipping =
        resmgr.get<dgm::Clip>("weapons.png.clip").value();
    const auto&& shader = resmgr.get<sf::Shader>("shader").value();

    sf::RectangleShape weaponSprite;
    weaponSprite.setTexture(&hud.get());
    const float size = screenWidth / 3.f;
    weaponSprite.setSize({ size, size });
    weaponSprite.setPosition(
        { screenWidth / 2.f - size / 2.f, screenHeight - size });

    return RenderContext {
        .text = createTextObject(
            resmgr.get<sf::Font>("pico-8.ttf").value(), 32u, sf::Color::White),
        .level = buildLevelRepresentation(resmgr.get<LevelD>(mapname).value()),
        .tilesetTexture = tileset.get(),
        .spritesheetTexture = spritesheet.get(),
        .tilesetClipping = tilesetClip.get(),
        .spritesheetClipping = spritesheetClip.get(),
        .weaponHudClipping = hudClipping.get(),
        .shader = shader.get(),
        .depthBuffer = std::vector<float>(screenWidth),
        .weaponSprite = weaponSprite
    };
}
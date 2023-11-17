#include "render/RenderContext.hpp"
#include "utils/Builder.hpp"
#include <LevelD.hpp>
#include <core/Enums.hpp>
#include <queue>
#include <utils/LightmapBuilder.hpp>

sf::Text RenderContextBuilder::createTextObject(
    const sf::Font& font, unsigned charSize, sf::Color color)
{
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(charSize);
    text.setFillColor(color);
    return text;
};

DrawableLevel
RenderContextBuilder::buildLevelRepresentation(const LevelD& level)
{
    auto bottomMesh = Builder::buildTextureMeshFromLvd(level, 0);
    auto upperMesh = Builder::buildTextureMeshFromLvd(level, 1);
    return DrawableLevel { .bottomTextures = bottomMesh,
                           .upperTextures = upperMesh,
                           .lightmap = LightmapBuilder::buildLightmap(
                               bottomMesh, upperMesh, level) };
}

sf::RectangleShape RenderContextBuilder::createWeaponSprite(
    const sf::Texture& texture,
    const unsigned screenWidth,
    const unsigned screenHeight)
{
    sf::RectangleShape weaponSprite;
    weaponSprite.setTexture(&texture);
    const float size = screenWidth / 3.f;
    weaponSprite.setSize({ size, size });
    weaponSprite.setPosition(
        { screenWidth / 2.f - size / 2.f, screenHeight - size });
    return weaponSprite;
}

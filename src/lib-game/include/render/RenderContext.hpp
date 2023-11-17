#pragma once

#include <DGM/dgm.hpp>
#include <LevelD.hpp>
#include <render/DrawableLevel.hpp>

class RenderContextBuilder
{
public:
    [[nodiscard]] static sf::RectangleShape createWeaponSprite(
        const sf::Texture& texture,
        const unsigned screenWidth,
        const unsigned screenHeight);

    [[nodiscard]] static sf::Text
    createTextObject(const sf::Font& font, unsigned charSize, sf::Color color);

    [[nodiscard]] static DrawableLevel
    buildLevelRepresentation(const LevelD& level);
};

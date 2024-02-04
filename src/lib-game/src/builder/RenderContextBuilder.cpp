#include "builder/RenderContextBuilder.hpp"
#include <LevelD.hpp>
#include <core/Enums.hpp>
#include <queue>

sf::Text RenderContextBuilder::createTextObject(
    const sf::Font& font, unsigned charSize, sf::Color color)
{
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(charSize);
    text.setFillColor(color);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(1.f);
    return text;
}

sf::RectangleShape RenderContextBuilder::createSkybox(
    const sf::Texture& texture, unsigned screenWidth, unsigned screenHeight)
{
    auto result = sf::RectangleShape(
        sf::Vector2f(sf::Vector2u(screenWidth * 2, screenHeight)));
    result.setTexture(&texture);
    return result;
}

sf::RectangleShape RenderContextBuilder::createHudIcon(
    const sf::Texture& texture, unsigned screenWidth)
{
    auto result =
        sf::RectangleShape({ screenWidth / 20.f, screenWidth / 20.f });
    result.setTexture(&texture);
    return result;
};

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

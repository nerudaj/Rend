#include "render/RenderContext.hpp"
#include "utils/Builder.hpp"
#include <LevelD.hpp>

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

RenderContext RenderContext::buildRenderContext(
    const dgm::ResourceManager& resmgr,
    const std::string& mapname,
    unsigned screenWidth,
    unsigned screenHeight)
{
    auto&& level = resmgr.get<LevelD>(mapname).value().get();
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
        .level = { .bottomTextures = Builder::buildTextureMeshFromLvd(level, 0),
                   .upperTextures =
                       Builder::buildTextureMeshFromLvd(level, 1) },
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
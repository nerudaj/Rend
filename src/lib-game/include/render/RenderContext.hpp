#pragma once

#include <DGM/dgm.hpp>
#include <core/Types.hpp>

struct DrawableLevel
{
    dgm::Mesh bottomTextures;
    dgm::Mesh upperTextures;
    LightmapType lightmap;
};

// Struct that should contain all primitives needed to render
// the Scene.
// Automatically constructed by the RenderingEngine using the
// buildRenderContext method
struct RenderContext
{
    sf::Text text;
    DrawableLevel level;
    const sf::Texture& tilesetTexture;
    const sf::Texture& spritesheetTexture;
    dgm::Clip tilesetClipping;
    dgm::Clip spritesheetClipping;
    dgm::Clip weaponHudClipping;
    const sf::Shader& shader;
    std::vector<float> depthBuffer;
    sf::RectangleShape weaponSprite;

    [[nodiscard]] static RenderContext buildRenderContext(
        const dgm::ResourceManager& resmgr,
        const std::string& mapname,
        unsigned screenWidth,
        unsigned screenHeight);
};

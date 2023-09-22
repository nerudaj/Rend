#pragma once

#include <DGM/dgm.hpp>

enum class TilesetMapping
{
    Floor1 = 0,
    Floor2,
    CeilSky = 2,
    CeilLow,
    Crate1 = 4,
    Crate2,
    Wall1 = 6,
    End = 16
};

struct DrawableLevel
{
    dgm::Mesh bottomTextures;
    dgm::Mesh upperTextures;
    // TODO: lightmap
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
    const sf::Shader& shader;
    std::vector<float> depthBuffer;

    [[nodiscard]] static RenderContext buildRenderContext(
        const dgm::ResourceManager& resmgr,
        const std::string& mapname,
        unsigned screenWidth);
};

#pragma once

#include <DGM/dgm.hpp>
#include <LevelD.hpp>

class MeshBuilder
{
public:
    [[nodiscard]] static dgm::Mesh
    buildMeshFromLvd(const LevelD& lvd, unsigned layerId = 0);

    [[nodiscard]] static dgm::Mesh
    buildTextureMeshFromLvd(const LevelD& lvd, unsigned layerId = 0);

    [[nodiscard]] static dgm::TileMap buildTileMapFromLvd(
        const sf::Texture& texture,
        dgm::UniversalReference<dgm::Clip> auto&& clip,
        const LevelD& lvd,
        unsigned layerId = 0)
    {
        auto&& result =
            dgm::TileMap(texture, std::forward<decltype(clip)>(clip));
        auto&& tiles = lvd.mesh.layers.at(layerId).tiles;
        result.build(
            { lvd.mesh.tileWidth, lvd.mesh.tileHeight },
            std::vector<int>(tiles.begin(), tiles.end()),
            { lvd.mesh.layerWidth, lvd.mesh.layerHeight });
        return result;
    }
};
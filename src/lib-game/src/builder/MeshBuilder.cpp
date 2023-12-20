#include "builder/MeshBuilder.hpp"

dgm::Mesh MeshBuilder::buildMeshFromLvd(const LevelD& lvd, unsigned layerIdx)
{
    return dgm::Mesh(
        lvd.mesh.layers.at(layerIdx).blocks,
        { lvd.mesh.layerWidth, lvd.mesh.layerHeight },
        { lvd.mesh.tileWidth, lvd.mesh.tileHeight });
}

dgm::Mesh
MeshBuilder::buildTextureMeshFromLvd(const LevelD& lvd, unsigned layerIdx)
{
    auto&& tiles = lvd.mesh.layers.at(layerIdx).tiles;
    return dgm::Mesh(
        std::vector<int>(tiles.begin(), tiles.end()),
        { lvd.mesh.layerWidth, lvd.mesh.layerHeight },
        { lvd.mesh.tileWidth, lvd.mesh.tileHeight });
}

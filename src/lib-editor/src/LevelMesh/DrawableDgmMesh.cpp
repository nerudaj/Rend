#include "LevelMesh/DrawableDgmMesh.hpp"
#include "Utilities/Utilities.hpp"

void DrawableDgmMesh::build(
    const std::vector<int>& data,
    const sf::Vector2u& dataSize,
    const sf::Vector2u& voxelSize)
{
    mesh = dgm::Mesh(data, dataSize, voxelSize);
    map.build(voxelSize, data, dataSize);
}

DrawableDgmMesh::DrawableDgmMesh()
{
    auto [image, clip] =
        Utilities::createChessboardTexture(sf::Color(0, 0, 255, 96));
    if (!texture.loadFromImage(image))
    {
        throw std::runtime_error("Cannot build overlay texture");
    }

    map = dgm::TileMap(texture, std::move(clip));

    // TODO: Test scenario
    // 1) open level 1
    // 2) turn on overlay
    // 3) open level 2
    // 4) check overlay still working
}

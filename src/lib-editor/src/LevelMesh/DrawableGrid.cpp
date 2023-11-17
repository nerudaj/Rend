#include "LevelMesh/DrawableGrid.hpp"
#include "Utilities/Utilities.hpp"

DrawableGrid::DrawableGrid()
{
    auto [image, clip] =
        Utilities::createChessboardTexture(sf::Color(255, 255, 255, 16));
    if (!texture.loadFromImage(image))
    {
        throw std::runtime_error("Cannot build overlay texture");
    }

    tilemap = dgm::TileMap(texture, std::move(clip));
}

void DrawableGrid::build(
    const sf::Vector2u& gridSize, const sf::Vector2u& tileSize)
{
    std::vector<int> bgrData(size_t(gridSize.x) * gridSize.y, 0);
    for (unsigned y = 0; y < gridSize.y; y++)
    {
        for (unsigned x = y % 2; x < gridSize.x; x += 2)
        {
            bgrData[size_t(y) * gridSize.x + x] = 1;
        }
    }

    tilemap.build(tileSize, bgrData, gridSize);
}

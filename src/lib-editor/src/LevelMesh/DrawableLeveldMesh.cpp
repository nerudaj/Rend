#include "LevelMesh/DrawableLeveldMesh.hpp"

DrawableLeveldMesh::DrawableLeveldMesh(sf::Texture& texture, dgm::Clip clip)
    : map(texture, clip)
{
}

void DrawableLeveldMesh::drawTo(
    tgui::CanvasSFML::Ptr canvas, bool renderCollisionMesh, bool isTopLevel)
{
    canvas->draw(map);

    if (!isTopLevel)
    {
        canvas->draw(darkOverlay);
    }

    if (isTopLevel)
    {
        if (renderCollisionMesh) mesh.drawTo(canvas);
        grid.drawTo(canvas);
    }
}

void DrawableLeveldMesh::build(
    const std::vector<int>& tiles,
    const std::vector<int>& blocks,
    const sf::Vector2u& dataSize)
{
    const auto tileSize = map.getClip().getFrameSize();
    map.build(tileSize, tiles, dataSize);
    mesh.build(blocks, dataSize, tileSize);
    grid.build(dataSize, tileSize);
    darkOverlay.setSize(sf::Vector2f(
        sf::Vector2u(tileSize.x * dataSize.x, tileSize.y * dataSize.y)));
    darkOverlay.setFillColor(sf::Color(0, 0, 0, 128));
}

#include "LevelMesh/DrawableLeveldMesh.hpp"

void DrawableLeveldMesh::drawTo(
    tgui::CanvasSFML::Ptr canvas, bool renderCollisionMesh)
{
    canvas->draw(map);
    if (renderCollisionMesh) mesh.drawTo(canvas);
    grid.drawTo(canvas);
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
}

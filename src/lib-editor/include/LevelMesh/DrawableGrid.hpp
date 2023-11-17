#pragma once

#include <DGM/classes/TileMap.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/tgui.hpp>

class DrawableGrid final
{
private:
    dgm::TileMap tilemap;
    sf::Texture texture;

public:
    DrawableGrid();
    DrawableGrid(DrawableGrid&&) = delete;
    DrawableGrid(const DrawableGrid&) = delete;

public:
    void drawTo(tgui::CanvasSFML::Ptr canvas)
    {
        canvas->draw(tilemap);
    }

    void build(const sf::Vector2u& gridSize, const sf::Vector2u& voxelSize);
};
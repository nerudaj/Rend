#pragma once

#include <DGM/classes/TileMap.hpp>
#include <TGUI/Widgets/Canvas.hpp>

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
	void drawTo(tgui::Canvas::Ptr canvas)
	{
		canvas->draw(tilemap);
	}

	void build(
		const sf::Vector2u& gridSize,
		const sf::Vector2u& voxelSize);
};
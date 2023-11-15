#pragma once

#include <DGM/classes/TileMap.hpp>

class PollableTileMap final : public dgm::TileMap
{
public:
	unsigned getTileValue(unsigned tileX, unsigned tileY) const;

	PollableTileMap() = default;
	PollableTileMap(sf::Texture& texture, dgm::Clip clip)
		: dgm::TileMap(texture, std::move(clip))
	{}
};
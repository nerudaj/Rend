#pragma once

#include <vector>
#include <LevelD.hpp>

struct TestTile
{
	unsigned x;
	unsigned y;
	unsigned value;
	bool solid;
};

class LeveldBuilder {
public:
	static LevelD buildWithMesh(
		unsigned width,
		unsigned height,
		std::vector<TestTile> tiles)
	{
		LevelD result;

		result.mesh.layerWidth = width;
		result.mesh.layerHeight = height;
		result.mesh.tileWidth = 32;
		result.mesh.tileHeight = 32;
		result.mesh.layers.resize(1);
		result.mesh.layers[0].tiles.resize(width * height, 0);
		result.mesh.layers[0].blocks.resize(width * height, 0);

		for (auto&& tile : tiles)
		{
			unsigned i = tile.y * width + tile.x;
			result.mesh.layers[0].tiles[i] = tile.value;
			result.mesh.layers[0].blocks[i] = tile.solid;
		}

		return result;
	}
};

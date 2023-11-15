#include "include/Utilities/CoordConverter.hpp"

TileRect CoordConverter::convertCoordToTileRect(const CoordRect& box) const noexcept
{
	return TileRect
	{
		.left = box.left / tileSize.x,
		.top = box.top / tileSize.y,
		.right = box.right / tileSize.x,
		.bottom = box.bottom / tileSize.y
	};
}

CoordRect CoordConverter::convertTileToCoordRect(
	TileRect const& box) const noexcept
{
	return CoordRect{
		.left = int(box.left * tileSize.x),
		.top = int(box.top * tileSize.y),
		.right = int((box.right + 1) * tileSize.x),
		.bottom = int((box.bottom + 1) * tileSize.y) };
}

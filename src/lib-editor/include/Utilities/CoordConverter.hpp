#pragma once

#include <SFML/System/Vector2.hpp>
#include "include/Utilities/Rect.hpp"

class CoordConverter final
{
public:
	[[nodiscard]]
	CoordConverter() = default;

	[[nodiscard]]
	CoordConverter(
		const sf::Vector2u& tileSize)
		: tileSize(tileSize)
	{}

public:
	[[nodiscard]]
	TileRect convertCoordToTileRect(
		CoordRect const& box) const noexcept;

	[[nodiscard]]
	CoordRect convertTileToCoordRect(
		TileRect const& box) const noexcept;

	[[nodiscard]]
	constexpr sf::Vector2u const& getTileSize() const noexcept
	{
		return tileSize;
	}

private:
	sf::Vector2u tileSize = { 1u, 1u };
};

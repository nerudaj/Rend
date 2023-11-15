#pragma once

#include "include/Utilities/Rect.hpp"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <DGM/classes/Clip.hpp>
#include <tuple>
#include <optional>

namespace Utilities
{
	std::tuple<sf::Image, dgm::Clip> createChessboardTexture(const sf::Color& accentColor);

	// Takes input integer vector and replaces any
	// negative coordinates with zeroes
	sf::Vector2i clipNegativeCoords(const sf::Vector2i& v);

	std::optional<TileRect> unifyRects(
		const std::optional<TileRect>& a,
		const std::optional<TileRect>& b) noexcept;

	sf::ConvexShape createArrow(
		const sf::Vector2i& from,
		const sf::Vector2i& to,
		const sf::Color& outlineColor,
		const float arrowThickness);
}
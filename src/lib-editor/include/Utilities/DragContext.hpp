#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>

struct DragContext
{
	/**
	 * Multiple points might be selected & dragged,
	 * but user starting dragging with mouse over one of them.
	 * Other points are dragged relative to how this particular
	 * point is dragged.
	 */
	std::size_t leadingItemId;

	/**
	 *  Vector from mouse position in the moment dragging started
	 *  to position of leading dragged object.
	 */
	sf::Vector2i initialDragOffset;
};

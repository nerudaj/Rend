#pragma once

#include "Utilities/Rect.hpp"
#include <SFML/System/Vector2.hpp>

class CoordConverter final
{
public:
    [[nodiscard]] CoordConverter() = default;

    [[nodiscard]] CoordConverter(const sf::Vector2u& tileSize)
        : tileSize(tileSize)
    {
    }

public:
    [[nodiscard]] TileRect
    convertCoordToTileRect(const CoordRect& box) const noexcept;

    [[nodiscard]] CoordRect
    convertTileToCoordRect(const TileRect& box) const noexcept;

    [[nodiscard]] constexpr const sf::Vector2u& getTileSize() const noexcept
    {
        return tileSize;
    }

private:
    sf::Vector2u tileSize = { 1u, 1u };
};

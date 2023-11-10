#pragma once

#include <SFML/System/Vector2.hpp>

struct Position
{
    sf::Vector2f value;
};

struct Direction
{
    sf::Vector2f value;
};

/*
Pixels
*/
[[nodiscard]] constexpr static float
operator""_px(unsigned long long value) noexcept
{
    return static_cast<float>(value);
}

/**
  Units Per Second

  Unit of length. Voxels are defined in units and units roughly equate
  to pixels. So for 16x16 px textures, voxel is 16x16 units.
 */
[[nodiscard]] constexpr static float
operator""_unitspersec(unsigned long long value) noexcept
{
    return static_cast<float>(value);
}
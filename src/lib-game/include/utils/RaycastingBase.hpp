#pragma once

#include <DGM/classes/Objects.hpp>
#include <SFML/System/Vector2.hpp>

class RaycastingBase
{
public:
    [[nodiscard]] RaycastingBase(const sf::Vector2u& _voxelSize) noexcept
        : voxelSize(sf::Vector2f(_voxelSize))
    {
    }

public:
    struct RaycastingState
    {
        sf::Vector2u tile;
        sf::Vector2i tileStep;
        sf::Vector2f rayStep;
        sf::Vector2f intercept;
    };

    [[nodiscard]] static RaycastingState
    computeInitialRaycastringStateFromPositionAndDirection(
        const sf::Vector2f& position, const sf::Vector2f& direction) noexcept;

    [[nodiscard]] dgm::Rect
    getTileBoundingBox(const sf::Vector2u& tile) const noexcept
    {
        return dgm::Rect(sf::Vector2f(tile) * voxelSize.x, voxelSize);
    }

protected:
    sf::Vector2f voxelSize;
};
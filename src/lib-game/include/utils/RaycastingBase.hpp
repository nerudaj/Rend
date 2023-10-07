#pragma once

#include <DGM/classes/Math.hpp>
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

protected:
    [[nodiscard]] static float getInterceptSize(
        bool side,
        const sf::Vector2f& intercept,
        const sf::Vector2f& rayStep) noexcept
    {
        return side == 0 ? intercept.x - rayStep.x : intercept.y - rayStep.y;
    }

    [[nodiscard]] static sf::Vector2f computeHitloc(
        const sf::Vector2f& cameraPos,
        const sf::Vector2f& rayDir,
        float interceptSize) noexcept
    {
        return cameraPos + dgm::Math::toUnit(rayDir) * interceptSize;
    }

    [[nodiscard]] static RaycastingState
    computeInitialRaycastringStateFromPositionAndDirection(
        const sf::Vector2f& position, const sf::Vector2f& direction) noexcept;

    [[nodiscard]] dgm::Rect
    getTileBoundingBox(const sf::Vector2u& tile) const noexcept
    {
        return dgm::Rect(sf::Vector2f(tile) * voxelSize.x, voxelSize);
    }

    bool advanceRay(
        sf::Vector2f& intercept,
        sf::Vector2u& tile,
        const sf::Vector2f& rayStep,
        const sf::Vector2i& tileStep) noexcept;

protected:
    sf::Vector2f voxelSize;
};
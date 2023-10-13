#pragma once

#include <core/Scene.hpp>
#include <cstddef>
#include <optional>
#include <utils/RaycastingBase.hpp>
#include <utils/SemanticTypes.hpp>

class Hitscanner final : public RaycastingBase
{
public:
    [[nodiscard]] Hitscanner(const Scene& scene) noexcept
        : scene(scene), RaycastingBase(scene.level.bottomMesh.getVoxelSize())
    {
    }

public:
    // Returns nearest enemy index that has been hit
    [[nodiscard]] HitscanResult hitscan(
        const Position& position,
        const Direction& direction,
        EntityIndexType idxToIgnore);

protected:
    [[nodiscard]] std::optional<EntityIndexType> findHitInCandidates(
        const std::vector<EntityIndexType>& candidateIdxs,
        const Position& position,
        const Direction& direction,
        EntityIndexType idxToIgnore);

    [[nodiscard]] sf::Vector2f getSpriteHitloc(
        const sf::Vector2f& casterPos, const sf::Vector2f& thingPos) noexcept
    {
        return thingPos - dgm::Math::toUnit(thingPos - casterPos) * 0.1f;
    }

private:
    const Scene& scene;
};
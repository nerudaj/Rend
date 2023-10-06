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
    [[nodiscard]] std::optional<EntityIndexType>
    hitscan(const Position& position, const Direction& direction);

    [[nodiscard]] std::optional<EntityIndexType> findHitInCandidates(
        const std::vector<EntityIndexType>& candidateIdxs,
        const Position& position,
        const Direction& direction);

private:
    const Scene& scene;
};
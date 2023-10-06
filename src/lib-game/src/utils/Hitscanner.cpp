#include <core/Constants.hpp>
#include <core/EntityTraits.hpp>
#include <utils/Hitscanner.hpp>

std::optional<EntityIndexType>
Hitscanner::hitscan(const Position& position, const Direction& direction)
{
    auto&& [tile, tileStep, rayStep, intercept] =
        computeInitialRaycastringStateFromPositionAndDirection(
            position.value / voxelSize.x, direction.value);

    while (true)
    {
        if (scene.level.bottomMesh.at(tile)) return std::nullopt;

        auto hit = findHitInCandidates(
            scene.spatialIndex.getOverlapCandidates(getTileBoundingBox(tile)),
            position,
            direction);
        if (hit) return hit;

        if (intercept.x < intercept.y)
        {
            intercept.x += rayStep.x;
            tile.x += tileStep.x;
        }
        else
        {
            intercept.y += rayStep.y;
            tile.y += tileStep.y;
        }
    }
}

std::optional<EntityIndexType> Hitscanner::findHitInCandidates(
    const std::vector<EntityIndexType>& candidateIdxs,
    const Position& position,
    const Direction& direction)
{
    auto aimLine = dgm::Math::Line(direction.value, position.value);
    auto candidateToIdxAndEntity =
        [&](EntityIndexType idx) -> std::pair<EntityIndexType, const Entity&> {
        return { idx, scene.things[idx] };
    };

    float minDistance = INFINITY;
    std::optional<EntityIndexType> result = std::nullopt;
    for (auto&& [idx, entity] :
         candidateIdxs | std::views::transform(candidateToIdxAndEntity))
    {
        if (isDestructible(entity.typeId)
            && dgm::Math::hasIntersection(aimLine, entity.hitbox))
        {
            const float distance = dgm::Math::getSize(
                entity.hitbox.getPosition() - position.value);
            if (distance < minDistance)
            {
                minDistance = distance;
                result = idx;
            }
        }
    }

    return result;
}

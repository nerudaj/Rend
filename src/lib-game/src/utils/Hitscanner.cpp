#include <core/Constants.hpp>
#include <core/EntityTraits.hpp>
#include <utils/Hitscanner.hpp>

HitscanResult Hitscanner::hitscan(
    const Position& position,
    const Direction& direction,
    EntityIndexType idxToIgnore)
{
    auto normalizedPos = position.value / voxelSize.x;
    auto&& [tile, tileStep, rayStep, intercept] =
        computeInitialRaycastringStateFromPositionAndDirection(
            normalizedPos, direction.value);

    bool side = 0;
    while (true)
    {
        if (scene.level.bottomMesh.at(tile))
            return { .impactPosition =
                         computeHitloc(
                             normalizedPos,
                             direction.value,
                             getInterceptSize(side, intercept, rayStep))
                         * voxelSize.x };

        auto hit = findHitInCandidates(
            scene.spatialIndex.getOverlapCandidates(getTileBoundingBox(tile)),
            position,
            direction,
            idxToIgnore);
        if (hit)
            return { .impactPosition = getSpriteHitloc(
                         position.value,
                         scene.things[hit.value()].hitbox.getPosition()),
                     .impactedEntityIdx = hit.value() };

        side = advanceRay(intercept, tile, rayStep, tileStep);
    }
}

std::optional<EntityIndexType> Hitscanner::findHitInCandidates(
    const std::vector<EntityIndexType>& candidateIdxs,
    const Position& position,
    const Direction& direction,
    EntityIndexType idxToIgnore)
{
    auto aimLine = dgm::Math::Line(direction.value, position.value);
    auto candidateToIdxAndEntity =
        [&](EntityIndexType idx) -> std::pair<EntityIndexType, const Entity&> {
        return { idx, scene.things[idx] };
    };

    float minDistance = INFINITY;
    std::optional<EntityIndexType> result = std::nullopt;
    for (auto&& [idx, entity] :
         candidateIdxs
             | std::views::filter([idxToIgnore](auto idx)
                                  { return idx != idxToIgnore; })
             | std::views::transform(candidateToIdxAndEntity))
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

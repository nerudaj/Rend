#include "engine/PhysicsEngine.hpp"
#include <utility>

void PhysicsEngine::update(const dgm::Time& time)
{
    for (auto&& [thing, id] : scene.things)
    {
        if (thing.typeId != EntityType::Player) continue;

        scene.spatialIndex.removeFromLookup(id, thing.hitbox);

        auto&& input = scene.inputs[thing.inputId];

        thing.direction = dgm::Math::getRotated(
            thing.direction,
            input.getSteer() * PLAYER_RADIAL_SPEED * time.getDeltaTime());

        auto&& forward =
            dgm::Math::toUnit(
                thing.direction * input.getThrust()
                + getPerpendicular(thing.direction) * input.getSidewardThrust())
            * PLAYER_FORWARD_SPEED * time.getDeltaTime();
        dgm::Collision::advanced(scene.level.bottomMesh, thing.hitbox, forward);
        thing.hitbox.move(forward);

        auto&& candidates =
            scene.spatialIndex.getOverlapCandidates(thing.hitbox);
        for (auto&& candidateId : candidates)
        {
            auto&& candidate = scene.things[candidateId];
            if (!isSolid(candidate.typeId)) continue;
            if (dgm::Collision::basic(thing.hitbox, candidate.hitbox))
                thing.hitbox.move(-forward);
        }

        scene.spatialIndex.returnToLookup(id, thing.hitbox);
    }
}

#include "engine/PhysicsEngine.hpp"
#include <events/EventQueue.hpp>
#include <utility>

void PhysicsEngine::update(const float deltaTime)
{
    for (auto&& [thing, id] : scene.things)
    {
        scene.spatialIndex.removeFromLookup(id, thing.hitbox);
        if (thing.typeId == EntityType::Player)
            handlePlayer(thing, deltaTime);
        else if (thing.typeId == EntityType::ProjectileRocket)
            handleProjectile(thing, id, deltaTime);
        scene.spatialIndex.returnToLookup(id, thing.hitbox);
    }
}

void PhysicsEngine::handlePlayer(Entity& thing, const float deltaTime)
{
    auto&& input = scene.inputs[thing.inputId];

    thing.direction = dgm::Math::getRotated(
        thing.direction, input.getSteer() * PLAYER_RADIAL_SPEED * deltaTime);

    auto&& forward =
        dgm::Math::toUnit(
            thing.direction * input.getThrust()
            + getPerpendicular(thing.direction) * input.getSidewardThrust())
        * PLAYER_FORWARD_SPEED * deltaTime;
    dgm::Collision::advanced(scene.level.bottomMesh, thing.hitbox, forward);
    thing.hitbox.move(forward);

    auto&& candidates = scene.spatialIndex.getOverlapCandidates(thing.hitbox);
    for (auto&& candidateId : candidates)
    {
        auto&& candidate = scene.things[candidateId];
        if (!isSolid(candidate.typeId)) continue;
        if (dgm::Collision::basic(thing.hitbox, candidate.hitbox))
            thing.hitbox.move(-forward);
    }
}

void PhysicsEngine::handleProjectile(
    Entity& thing, std::size_t id, const float deltaTime)
{
    auto&& forward = thing.direction * PROJECTILE_FORWARD_SPEED * deltaTime;
    thing.hitbox.move(forward);

    const auto hasCollided = [&]() -> bool
    {
        if (dgm::Collision::basic(scene.level.bottomMesh, thing.hitbox))
        {
            return true;
        }

        auto&& candidateIds =
            scene.spatialIndex.getOverlapCandidates(thing.hitbox);
        for (auto&& candidateId : candidateIds)
        {
            auto&& candidate = scene.things[candidateId];
            if (isSolid(candidate.typeId)
                && dgm::Collision::basic(thing.hitbox, candidate.hitbox))
                return true;
        }

        return false;
    }();

    if (hasCollided)
    {
        EventQueue::add<ProjectileDestroyedGameEvent>(id);
    }
}

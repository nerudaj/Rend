#include "engine/PhysicsEngine.hpp"
#include <core/EntityDefinitions.hpp>
#include <core/EntityTraits.hpp>
#include <events/EventQueue.hpp>
#include <utility>

void PhysicsEngine::update(const float deltaTime)
{
    for (auto&& [thing, id] : scene.things)
    {
        scene.spatialIndex.removeFromLookup(id, thing.hitbox);
        if (thing.typeId == EntityType::Player)
            handlePlayer(thing, deltaTime);
        else if (isProjectile(thing.typeId))
            handleProjectile(thing, id, deltaTime);
        scene.spatialIndex.returnToLookup(id, thing.hitbox);
    }
}

void PhysicsEngine::handlePlayer(Entity& thing, const float deltaTime)
{
    const auto& input = scene.playerStates[thing.stateIdx].input;
    const auto& def = ENTITY_PROPERTIES.at(thing.typeId);

    thing.direction = dgm::Math::toUnit(dgm::Math::getRotated(
        thing.direction, input.getSteer() * PLAYER_RADIAL_SPEED * deltaTime));

    auto&& forward =
        dgm::Math::toUnit(
            thing.direction * input.getThrust()
            + getPerpendicular(thing.direction) * input.getSidewardThrust())
        * def.speed * deltaTime;
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
    const auto& def = ENTITY_PROPERTIES.at(thing.typeId);
    auto&& forward = thing.direction * def.speed * deltaTime;

    const auto hasCollided = [&]() -> bool
    {
        if (dgm::Collision::advanced(
                scene.level.bottomMesh, thing.hitbox, forward))
        {
            if (def.isBouncy && thing.health > 0)
            {
                thing.health -= 50;
                // invert components of direction vector that caused collision
                thing.direction.x *= forward.x == 0.f ? -1.f : 1.f;
                thing.direction.y *= forward.y == 0.f ? -1.f : 1.f;
                eventQueue->emplace<SoundTriggeredAudioEvent>(
                    def.specialSound,
                    SoundSourceType::Ambient,
                    thing.stateIdx,
                    thing.hitbox.getPosition());
                return false;
            }
            return true;
        }

        thing.hitbox.move(forward);

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
        eventQueue->emplace<ProjectileDestroyedGameEvent>(id);
    }
}

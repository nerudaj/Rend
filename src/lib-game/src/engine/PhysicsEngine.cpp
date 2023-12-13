#include "engine/PhysicsEngine.hpp"
#include <core/EntityDefinitions.hpp>
#include <events/EventQueue.hpp>
#include <utility>
#include <utils/MathHelpers.hpp>

void PhysicsEngine::update(const float deltaTime)
{
    for (auto&& [thing, id] : scene.things)
    {
        const auto& properties = ENTITY_PROPERTIES.at(thing.typeId);
        scene.spatialIndex.removeFromLookup(id, thing.hitbox);
        if (thing.typeId == EntityType::Player)
            handlePlayer(properties, thing, deltaTime);
        else if (properties.traits & Trait::Projectile)
            handleProjectile(properties, thing, id, deltaTime);
        scene.spatialIndex.returnToLookup(id, thing.hitbox);
    }
}

void PhysicsEngine::handlePlayer(
    const EntityProperties& properties, Entity& thing, const float deltaTime)
{
    const auto& input = scene.playerStates[thing.stateIdx].input;

    thing.direction = dgm::Math::toUnit(dgm::Math::getRotated(
        thing.direction, input.getSteer() * PLAYER_RADIAL_SPEED * deltaTime));

    auto&& forward =
        dgm::Math::toUnit(
            thing.direction * input.getThrust()
            + getPerpendicular(thing.direction) * input.getSidewardThrust())
        * properties.speed * deltaTime;
    dgm::Collision::advanced(scene.level.bottomMesh, thing.hitbox, forward);
    thing.hitbox.move(forward);

    auto&& candidates = scene.spatialIndex.getOverlapCandidates(thing.hitbox);
    for (auto&& candidateId : candidates)
    {
        auto&& candidate = scene.things[candidateId];
        const bool isSolid =
            ENTITY_PROPERTIES.at(candidate.typeId).traits & Trait::Solid;
        if (!isSolid) continue;
        if (dgm::Collision::basic(thing.hitbox, candidate.hitbox))
            thing.hitbox.move(-forward);
    }
}

void PhysicsEngine::handleProjectile(
    const EntityProperties& properties,
    Entity& thing,
    std::size_t id,
    const float deltaTime)
{
    auto&& forward = thing.direction * properties.speed * deltaTime;

    const auto hasCollided = [&]() -> bool
    {
        if (dgm::Collision::advanced(
                scene.level.bottomMesh, thing.hitbox, forward))
        {
            if (properties.traits & Trait::Bouncy && thing.health > 0)
            {
                thing.health -= 50;
                // invert components of direction vector that caused collision
                thing.direction.x *= forward.x == 0.f ? -1.f : 1.f;
                thing.direction.y *= forward.y == 0.f ? -1.f : 1.f;
                eventQueue->emplace<SoundTriggeredAudioEvent>(
                    properties.specialSound,
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
            const bool isSolid =
                ENTITY_PROPERTIES.at(candidate.typeId).traits & Trait::Solid;
            if (isSolid
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

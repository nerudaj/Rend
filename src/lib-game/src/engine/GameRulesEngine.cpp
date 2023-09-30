#include "engine/GameRulesEngine.hpp"
#include "events/EventQueue.hpp"
#include <utils/SceneBuilder.hpp>

#pragma region EventHandling

void GameRulesEngine::operator()(const PickablePickedUpGameEvent& e)
{
    scene.things.emplaceBack(Entity {
        .typeId = EntityType::MarkerItemRespawner,
        .hitbox = scene.things[e.entityIndex].hitbox,

        // Not the best code I've written - reusing random variables
        // to encode extra information -_-
        .inputId = static_cast<int>(scene.things[e.entityIndex].typeId),
        .health = 0, // marker for spawn effect
        .fireCooldown = ITEM_RESPAWN_TIMEOUT,
    });

    removeEntity(e.entityIndex);
}

void GameRulesEngine::operator()(const ProjectileCreatedGameEvent& e)
{
    scene.things.emplaceBack(SceneBuilder::createProjectile(
        e.type,
        Position { e.position },
        Direction { e.direction },
        scene.frameId));
}

void GameRulesEngine::operator()(const ProjectileDestroyedGameEvent& e)
{
    const auto&& explosionHitbox =
        dgm::Circle(scene.things[e.entityIndex].hitbox.getPosition(), 8_px);

    // Delete projectile
    removeEntity(e.entityIndex);

    // Damage all destructibles in vicinity
    for (auto&& [candidateId, candidate] :
         getOverlapCandidates(explosionHitbox))
    {
        if (isDestructible(candidate.typeId)
            && dgm::Collision::basic(explosionHitbox, candidate.hitbox))
        {
            damage(candidate, candidateId, ROCKET_DAMAGE);
        }
    }

    // Spawn explosion effect
    scene.things.emplaceBack(SceneBuilder::createEffect(
        EntityType::EffectExplosion,
        Position { explosionHitbox.getPosition() },
        scene.frameId));
}

void GameRulesEngine::operator()(const EntityDestroyedGameEvent& e)
{
    const auto thing = scene.things[e.entityIndex];
    const bool playerWasDestroyed = thing.typeId == EntityType::Player;

    if (playerWasDestroyed)
    {
        scene.things.emplaceBack(SceneBuilder::createEffect(
            EntityType::EffectDyingPlayer,
            Position { thing.hitbox.getPosition() },
            scene.frameId));

        auto idx = scene.things.emplaceBack(
            Entity { .typeId = EntityType::MarkerDeadPlayer,
                     .spriteClipIndex = SpriteId::NoRender,
                     .hitbox = thing.hitbox,
                     .direction = thing.direction,
                     .inputId = thing.inputId });

        if (scene.playerId == e.entityIndex) scene.playerId = idx;
    }

    removeEntity(e.entityIndex);
}

void GameRulesEngine::operator()(const PlayerRespawnedGameEvent& e)
{
    const auto thing = scene.things[e.entityIndex];
    const auto spawnPosition = getBestSpawnPosition();
    const auto spawnDirection = getBestSpawnDirection(spawnPosition);

    auto idx = scene.things.emplaceBack(SceneBuilder::createPlayer(
        Position { spawnPosition },
        Direction { spawnDirection },
        thing.inputId));

    if (e.entityIndex == scene.playerId) scene.playerId = idx;

    removeEntity(e.entityIndex);
}

void GameRulesEngine::operator()(const EffectSpawnedGameEvent& e)
{
    scene.things.emplaceBack(SceneBuilder::createEffect(
        e.type, Position { e.position }, scene.frameId));
}

void GameRulesEngine::operator()(const PickupSpawnedGameEvent& e)
{
    scene.things.emplaceBack(
        SceneBuilder::createPickup(e.type, Position { e.position }));
}

#pragma endregion

void GameRulesEngine::update(const float deltaTime)
{
    for (auto&& [thing, id] : scene.things)
    {
        switch (thing.typeId)
        {
        case EntityType::Player:
            handlePlayer(thing, id, deltaTime);
            break;
        case EntityType::MarkerDeadPlayer:
            handleDeadPlayer(thing, id);
            break;
        case EntityType::MarkerItemRespawner:
            handleItemRespawner(thing, id, deltaTime);
            break;
        default:
            break;
        }
    }
}

void GameRulesEngine::handlePlayer(
    Entity& thing, std::size_t id, const float deltaTime)
{
    scene.spatialIndex.removeFromLookup(id, thing.hitbox);

    for (auto&& [candidateId, candidate] : getOverlapCandidates(thing.hitbox))
    {
        if (isPickable(candidate.typeId)
            && dgm::Collision::basic(thing.hitbox, candidate.hitbox))
        {
            handleGrabbedPickable(thing, candidate, candidateId);
        }
    }

    scene.spatialIndex.returnToLookup(id, thing.hitbox);

    if (thing.fireCooldown > 0.f)
        thing.fireCooldown -= deltaTime;
    else if (scene.inputs[thing.inputId].isShooting())
    {
        thing.fireCooldown = WEAPON_LAUNCHER_COOLDOWN;
        auto position = thing.hitbox.getPosition()
                        + thing.direction * thing.hitbox.getRadius() * 2.f;
        EventQueue::add<ProjectileCreatedGameEvent>(ProjectileCreatedGameEvent(
            EntityType::ProjectileRocket, position, thing.direction));
    }
}

void GameRulesEngine::handleDeadPlayer(Entity& thing, std::size_t id)
{
    if (scene.inputs[thing.inputId].isShooting())
    {
        EventQueue::add<PlayerRespawnedGameEvent>(id);
    }
}

void GameRulesEngine::handleItemRespawner(
    Entity& thing, std::size_t idx, const float deltaTime)
{
    thing.fireCooldown -= deltaTime;
    if (thing.fireCooldown <= ITEM_SPAWN_EFFECT_TIMEOUT && thing.health == 0)
    {
        EventQueue::add<EffectSpawnedGameEvent>(EffectSpawnedGameEvent(
            EntityType::EffectSpawn, thing.hitbox.getPosition()));
        thing.health = 1;
    }
    else if (thing.fireCooldown <= 0.f)
    {
        EventQueue::add<PickupSpawnedGameEvent>(PickupSpawnedGameEvent(
            static_cast<EntityType>(thing.inputId),
            thing.hitbox.getPosition()));
        EventQueue::add<EntityDestroyedGameEvent>(idx);
    }
}

void GameRulesEngine::handleGrabbedPickable(
    Entity& grabber, Entity pickup, std::size_t pickupId)
{
    if (give(grabber, pickup.typeId) && !isWeaponPickable(pickup.typeId))
    {
        EventQueue::add<PickablePickedUpGameEvent>(pickupId);
    }
}

bool GameRulesEngine::give(Entity& thing, EntityType pickupId)
{
    using enum EntityType;
    switch (pickupId)
    {
    case PickupHealth:
        if (thing.health == MAX_BASE_HEALTH) return false;
        thing.health = std::clamp(
            thing.health + MEDIKIT_HEALTH_AMOUNT, 0, MAX_BASE_HEALTH);
        break;
    case PickupArmor:
        if (thing.armor == MAX_ARMOR) return false;
        thing.armor =
            std::clamp(thing.armor + ARMORSHARD_ARMOR_AMOUNT, 0, MAX_ARMOR);
        break;
    case PickupMegaHealth:
        thing.health = std::clamp(
            thing.health + MEGAHEALTH_HEALTH_AMOUNT, 0, MAX_UPPED_HEALTH);
        break;
    case PickupMegaArmor:
        thing.armor =
            std::clamp(thing.health + MEGAARMOR_ARMOR_AMOUNT, 0, MAX_ARMOR);
        break;
    case PickupBullets:
        if (thing.bulletCount == MAX_BULLETS) return false;
        thing.bulletCount =
            std::clamp(thing.bulletCount + BULLET_AMOUNT, 0, MAX_BULLETS);
        break;
    case PickupShells:
        if (thing.shellCount == MAX_SHELLS) return false;
        thing.shellCount =
            std::clamp(thing.shellCount + SHELL_AMOUNT, 0, MAX_SHELLS);
        break;
    case PickupEnergy:
        if (thing.energyCount == MAX_ENERGY) return false;
        thing.energyCount =
            std::clamp(thing.energyCount + ENERGY_AMOUNT, 0, MAX_ENERGY);
        break;
    case PickupRockets:
        if (thing.rocketCount == MAX_ROCKETS) return false;
        thing.rocketCount =
            std::clamp(thing.rocketCount + ROCKET_AMOUNT, 0, MAX_ROCKETS);
        break;
    default:
        return false;
    }

    return true;
}

void GameRulesEngine::damage(Entity& thing, std::size_t thingIndex, int damage)
{
    // TODO: max 2/3 of armor above 100?
    // Max 1/3 of the damage can be absorbed by the armor
    int amountAbsorbedByArmor = std::min(thing.armor, damage / 3);
    thing.armor -= amountAbsorbedByArmor;
    damage -= amountAbsorbedByArmor;

    thing.health -= damage;
    if (thing.health <= 0)
    {
        EventQueue::add<EntityDestroyedGameEvent>(thingIndex);
    }
}

#pragma region Helpers

void GameRulesEngine::removeEntity(std::size_t index)
{
    scene.spatialIndex.removeFromLookup(index, scene.things[index].hitbox);
    scene.things.eraseAtIndex(index);
}

sf::Vector2f GameRulesEngine::getBestSpawnPosition() const noexcept
{
    float bestDistance = 0.f;
    std::size_t bestSpawnIdx = 0;
    for (std::size_t i = 0; i < scene.spawns.size(); ++i)
    {
        float minDistance = INFINITY;
        for (auto&& [thing, idx] : scene.things)
        {
            if (thing.typeId != EntityType::Player) continue;

            minDistance = std::min(
                minDistance,
                dgm::Math::getSize(
                    scene.spawns[i] - thing.hitbox.getPosition()));
        }

        if (bestDistance < minDistance)
        {
            bestDistance = minDistance;
            bestSpawnIdx = i;
        }
    }

    return scene.spawns[bestSpawnIdx];
}

sf::Vector2f GameRulesEngine::getBestSpawnDirection(
    const sf::Vector2f& spawnPosition) const noexcept
{
    return dgm::Math::toUnit(
        scene.spatialIndex.getBoundingBox().getCenter() - spawnPosition);
}

#pragma endregion

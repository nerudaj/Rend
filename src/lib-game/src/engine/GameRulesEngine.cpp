#include "engine/GameRulesEngine.hpp"
#include "events/EventQueue.hpp"
#include <core/EntityTraits.hpp>
#include <utils/SceneBuilder.hpp>

#pragma region EventHandling

void GameRulesEngine::operator()(const PickablePickedUpGameEvent& e)
{
    scene.markers.emplaceBack(MarkerItemRespawner {
        .timeout = ITEM_RESPAWN_TIMEOUT,
        .pickupType = scene.things[e.entityIndex].typeId,
        .position = scene.things[e.entityIndex].hitbox.getPosition() });

    removeEntity(e.entityIndex);
}

void GameRulesEngine::operator()(const ProjectileCreatedGameEvent& e)
{
    scene.things.emplaceBack(SceneBuilder::createProjectile(
        e.type,
        Position { e.position },
        Direction { e.direction },
        scene.tick));
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
        scene.tick));
}

void GameRulesEngine::operator()(const EntityDestroyedGameEvent& e)
{
    removeEntity(e.entityIndex);
}

void GameRulesEngine::operator()(const PlayerRespawnedGameEvent& e)
{
    const auto& marker =
        std::get<MarkerDeadPlayer>(scene.markers[e.markerIndex]);
    const auto spawnPosition = getBestSpawnPosition();
    const auto spawnDirection = getBestSpawnDirection(spawnPosition);

    auto idx = scene.things.emplaceBack(SceneBuilder::createPlayer(
        Position { spawnPosition },
        Direction { spawnDirection },
        marker.stateId));
    scene.playerStates[scene.things[idx].stateId].inventory =
        SceneBuilder::getDefaultInventory(idx);

    if (marker.rebindCamera) scene.playerId = idx;

    scene.markers.eraseAtIndex(e.markerIndex);
}

void GameRulesEngine::operator()(const EffectSpawnedGameEvent& e)
{
    scene.things.emplaceBack(SceneBuilder::createEffect(
        e.type, Position { e.position }, scene.tick));
}

void GameRulesEngine::operator()(const PickupSpawnedGameEvent& e)
{
    scene.things.emplaceBack(
        SceneBuilder::createPickup(e.type, Position { e.position }));
    scene.markers.eraseAtIndex(e.markerIndex);
}

void GameRulesEngine::operator()(const HitscanProjectileFiredGameEvent& e)
{
    scene.things.emplaceBack(SceneBuilder::createDecal(
        e.hit.impactedEntityIdx.has_value(),
        Position { e.hit.impactPosition },
        scene.tick));

    if (!e.hit.impactedEntityIdx) return;

    damage(
        scene.things[e.hit.impactedEntityIdx.value()],
        e.hit.impactedEntityIdx.value(),
        e.damage);
}

void GameRulesEngine::operator()(ScriptTriggeredGameEvent e)
{
    auto& thing = scene.things[e.targetEntityIdx];
    const auto position =
        Position { thing.hitbox.getPosition()
                   + thing.direction * thing.hitbox.getRadius() * 2.f };
    auto& inventory = scene.playerStates[thing.stateId].inventory;

    switch (e.scriptId)
    {
        using enum ScriptId;
    case FireFlare:
        fireFlare(position, Direction { thing.direction }, inventory);
        break;
    case FirePellets:
        firePellets(
            position,
            Direction { thing.direction },
            inventory,
            e.targetEntityIdx);
        break;
    case FireBullet:
        fireBullet(
            position,
            Direction { thing.direction },
            inventory,
            e.targetEntityIdx);
        break;
    case FireLaserDart:
        fireLaserDart(position, Direction { thing.direction }, inventory);
        break;
    }
}

#pragma endregion

template<class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};

void GameRulesEngine::update(const float deltaTime)
{
    indicesToRemove.clear();

    for (auto&& [thing, id] : scene.things)
    {
        switch (thing.typeId)
        {
        case EntityType::Player:
            handlePlayer(thing, id);
            break;
        default:
            break;
        }

        if (isDestructible(thing.typeId) && thing.health <= 0)
        {
            removeEntity(id);
        }
    }

    for (auto&& [thing, idx] : scene.markers)
    {
        std::visit(
            overloaded { [&](MarkerItemRespawner& marker)
                         { handleItemRespawner(marker, idx, deltaTime); },
                         [&](MarkerDeadPlayer& marker)
                         { handleDeadPlayer(marker, idx); } },
            thing);
    }
}

void GameRulesEngine::deleteMarkedObjects()
{
    std::sort(indicesToRemove.begin(), indicesToRemove.end());
    for (unsigned i = 0; i < indicesToRemove.size(); i++)
    {
        if (i == 0 || indicesToRemove[i - 1] != indicesToRemove[i])
        {
            scene.things.eraseAtIndex(indicesToRemove[i]);
            std::cout << std::format(
                "{}: truly erased {}", scene.tick, indicesToRemove[i])
                      << std::endl;
        }
    }
}

void GameRulesEngine::handlePlayer(Entity& thing, std::size_t idx)
{
    auto& state = scene.playerStates[thing.stateId];

    scene.spatialIndex.removeFromLookup(idx, thing.hitbox);

    for (auto&& [candidateId, candidate] : getOverlapCandidates(thing.hitbox))
    {
        if (isPickable(candidate.typeId)
            && dgm::Collision::basic(thing.hitbox, candidate.hitbox))
        {
            handleGrabbedPickable(
                thing, state.inventory, candidate, candidateId);
        }
    }

    // No weapon interaction allowed when weapon is not in idle
    if (state.inventory.animationContext.animationStateId
        == AnimationStateId::Idle)
    {
        if (state.input.isShooting() && canFireActiveWeapon(state.inventory))
        {
            EventQueue::add<PlayerFiredAnimationEvent>(idx);
        }
        else if (state.input.shouldSwapToPreviousWeapon())
        {
            swapToPreviousWeapon(state.inventory, idx);
        }
        else if (state.input.shouldSwapToNextWeapon())
        {
            swapToNextWeapon(state.inventory, idx);
        }
        else if (state.input.shouldSwapToLastWeapon())
        {
        }
    }

    scene.spatialIndex.returnToLookup(idx, thing.hitbox);
}

void GameRulesEngine::handleDeadPlayer(
    MarkerDeadPlayer& marker, std::size_t idx)
{
    if (scene.playerStates[marker.stateId].input.isShooting())
    {
        EventQueue::add<PlayerRespawnedGameEvent>(idx);
    }
}

void GameRulesEngine::handleItemRespawner(
    MarkerItemRespawner& marker, std::size_t idx, const float deltaTime)
{
    marker.timeout -= deltaTime;
    if (marker.timeout <= ITEM_SPAWN_EFFECT_TIMEOUT
        && !marker.spawnEffectEmitted)
    {
        EventQueue::add<EffectSpawnedGameEvent>(
            EffectSpawnedGameEvent(EntityType::EffectSpawn, marker.position));
        marker.spawnEffectEmitted = true;
    }
    else if (marker.timeout <= 0.f)
    {
        EventQueue::add<PickupSpawnedGameEvent>(
            PickupSpawnedGameEvent(marker.pickupType, marker.position, idx));
    }
}

void GameRulesEngine::handleGrabbedPickable(
    Entity& entity,
    PlayerInventory& inventory,
    Entity pickup,
    std::size_t pickupId)
{
    if (give(entity, inventory, pickup.typeId)
        && !isWeaponPickable(pickup.typeId))
    {
        EventQueue::add<PickablePickedUpGameEvent>(pickupId);
    }
}

bool GameRulesEngine::canFireActiveWeapon(PlayerInventory& inventory) noexcept
{
    switch (inventory.activeWeaponType)
    {
        using enum EntityType;
    case WeaponFlaregun:
        return inventory.rocketCount > 0;
    case WeaponShotgun:
        return inventory.shellCount > 0;
    case WeaponTrishot:
        return inventory.bulletCount > 0;
    case WeaponCrossbow:
        return inventory.energyCount > 0;
    }

    return false;
}

void GameRulesEngine::swapToPreviousWeapon(
    PlayerInventory& inventory, EntityIndexType idx)
{
    inventory.activeWeaponType = weaponIndexToType(getPrevToggledBit(
        weaponTypeToIndex(inventory.activeWeaponType),
        inventory.acquiredWeapons));
    EventQueue::add<WeaponSwappedAnimationEvent>(idx);
}

void GameRulesEngine::swapToNextWeapon(
    PlayerInventory& inventory, EntityIndexType idx)
{
    inventory.activeWeaponType = weaponIndexToType(getNextToggledBit(
        weaponTypeToIndex(inventory.activeWeaponType),
        inventory.acquiredWeapons));
    EventQueue::add<WeaponSwappedAnimationEvent>(idx);
}

bool GameRulesEngine::give(
    Entity& entity, PlayerInventory& inventory, EntityType pickupId)
{
    using enum EntityType;
    switch (pickupId)
    {
    case PickupHealth:
        if (entity.health == MAX_BASE_HEALTH) return false;
        entity.health = std::clamp(
            entity.health + MEDIKIT_HEALTH_AMOUNT, 0, MAX_BASE_HEALTH);
        break;
    case PickupArmor:
        if (entity.armor == MAX_ARMOR) return false;
        entity.armor =
            std::clamp(entity.armor + ARMORSHARD_ARMOR_AMOUNT, 0, MAX_ARMOR);
        break;
    case PickupMegaHealth:
        entity.health = std::clamp(
            entity.health + MEGAHEALTH_HEALTH_AMOUNT, 0, MAX_UPPED_HEALTH);
        break;
    case PickupMegaArmor:
        entity.armor =
            std::clamp(entity.health + MEGAARMOR_ARMOR_AMOUNT, 0, MAX_ARMOR);
        break;
    case PickupBullets:
        if (inventory.bulletCount == MAX_BULLETS) return false;
        inventory.bulletCount =
            std::clamp(inventory.bulletCount + BULLET_AMOUNT, 0, MAX_BULLETS);
        break;
    case PickupShells:
        if (inventory.shellCount == MAX_SHELLS) return false;
        inventory.shellCount =
            std::clamp(inventory.shellCount + SHELL_AMOUNT, 0, MAX_SHELLS);
        break;
    case PickupEnergy:
        if (inventory.energyCount == MAX_ENERGY) return false;
        inventory.energyCount =
            std::clamp(inventory.energyCount + ENERGY_AMOUNT, 0, MAX_ENERGY);
        break;
    case PickupRockets:
        if (inventory.rocketCount == MAX_ROCKETS) return false;
        inventory.rocketCount =
            std::clamp(inventory.rocketCount + ROCKET_AMOUNT, 0, MAX_ROCKETS);
        break;
    case PickupShotgun: {
        constexpr auto index = weaponTypeToIndex(EntityType::WeaponShotgun);
        if (inventory.acquiredWeapons[index]) return false;
        inventory.shellCount = SHELL_AMOUNT;
        inventory.acquiredWeapons[index] = true;
        return false;
    }
    case PickupTrishot: {
        constexpr auto index = weaponTypeToIndex(EntityType::WeaponTrishot);
        if (inventory.acquiredWeapons[index]) return false;
        inventory.bulletCount = BULLET_AMOUNT;
        inventory.acquiredWeapons[index] = true;
        return false;
    }
    case PickupCrossbow: {
        constexpr auto index = weaponTypeToIndex(EntityType::WeaponCrossbow);
        if (inventory.acquiredWeapons[index]) return false;
        inventory.energyCount = ENERGY_AMOUNT;
        inventory.acquiredWeapons[index] = true;
        return false;
    }
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
}

#pragma region Helpers

void GameRulesEngine::removeEntity(std::size_t index)
{
    const auto thing = scene.things[index];
    std::cout << std::format(
        "{}: removeEntity({}) with type {}",
        scene.tick,
        index,
        static_cast<int>(thing.typeId))
              << std::endl;
    const bool playerWasDestroyed = thing.typeId == EntityType::Player;

    if (playerWasDestroyed)
    {
        auto idx = scene.things.emplaceBack(SceneBuilder::createEffect(
            EntityType::EffectDyingPlayer,
            Position { thing.hitbox.getPosition() },
            scene.tick));
        scene.things[idx].direction = thing.direction;

        bool rebindCamera = scene.playerId == index;
        scene.markers.emplaceBack(MarkerDeadPlayer {
            .rebindCamera = rebindCamera, .stateId = thing.stateId });

        if (rebindCamera) scene.playerId = idx;
    }

    scene.spatialIndex.removeFromLookup(index, thing.hitbox);
    std::cout << std::format(
        "{}: removedFromLookup({}) with hitbox {}",
        scene.tick,
        index,
        dgm::Utility::to_string(thing.hitbox.getPosition()))
              << std::endl;
    indicesToRemove.push_back(index);
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

void GameRulesEngine::fireFlare(
    const Position& position,
    const Direction& direction,
    PlayerInventory& inventory)
{
    assert(inventory.rocketCount);
    --inventory.rocketCount;
    EventQueue::add<ProjectileCreatedGameEvent>(ProjectileCreatedGameEvent(
        EntityType::ProjectileRocket, position.value, direction.value));
}

void GameRulesEngine::firePellets(
    const Position& position,
    const Direction& direction,
    PlayerInventory& inventory,
    EntityIndexType playerIdx)
{
    assert(inventory.shellCount);
    --inventory.shellCount;

    forEachDirection(
        direction.value,
        getPerpendicular(direction.value) * SHOTGUN_SPREAD,
        SHOTGUN_PELLET_AMOUNT,
        [&](const sf::Vector2f& hitscanDir)
        {
            auto hit = hitscanner.hitscan(
                position, Direction { hitscanDir }, playerIdx);
            EventQueue::add<HitscanProjectileFiredGameEvent>(
                HitscanProjectileFiredGameEvent(hit, SHELL_DAMAGE));
        });
}

void GameRulesEngine::fireBullet(
    const Position& position,
    const Direction& direction,
    PlayerInventory& inventory,
    EntityIndexType playerIdx)
{
    assert(inventory.bulletCount);
    --inventory.bulletCount;
    auto hit = hitscanner.hitscan(position, direction, playerIdx);
    EventQueue::add<HitscanProjectileFiredGameEvent>(
        HitscanProjectileFiredGameEvent(hit, TRISHOT_BULLET_DAMAGE));
}

void GameRulesEngine::fireLaserDart(
    const Position& position,
    const Direction& direction,
    PlayerInventory& inventory)
{
    assert(inventory.energyCount);
    --inventory.energyCount;
    EventQueue::add<ProjectileCreatedGameEvent>(ProjectileCreatedGameEvent(
        EntityType::ProjectileLaserDart, position.value, direction.value));
}

#pragma endregion

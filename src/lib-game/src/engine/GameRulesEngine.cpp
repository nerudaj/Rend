#include "engine/GameRulesEngine.hpp"
#include "events/EventQueue.hpp"
#include <core/EntityDefinitions.hpp>
#include <core/EntityTraits.hpp>
#include <utils/GameLogicHelpers.hpp>
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
        scene.tick,
        e.originatorStateIdx));
}

void GameRulesEngine::operator()(const ProjectileDestroyedGameEvent& e)
{
    const auto& projectile = scene.things[e.entityIndex];
    const auto& DEF = ENTITY_PROPERTIES.at(projectile.typeId);
    const auto hitbox =
        DEF.isExplosive
            ? dgm::Circle(projectile.hitbox.getPosition(), DEF.explosionRadius)
            : projectile.hitbox;

    // Damage all destructibles in vicinity
    for (auto&& [candidateId, candidate] : getOverlapCandidates(hitbox))
    {
        if (isDestructible(candidate.typeId)
            && dgm::Collision::basic(hitbox, candidate.hitbox))
        {
            const int damageAmount = DEF.isExplosive ? static_cast<int>(
                                         DEF.damage
                                         / std::pow(
                                             getNormalizedDistanceToEpicenter(
                                                 candidate.hitbox, hitbox),
                                             2.f))
                                                     : DEF.damage;
            assert(damageAmount > 0);

            damage(
                candidate,
                candidateId,
                damageAmount,
                scene.things[e.entityIndex].stateIdx);
        }
    }

    // Delete projectile
    removeEntity(e.entityIndex);
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
        marker.stateIdx));

    auto& inventory = scene.playerStates[scene.things[idx].stateIdx].inventory;
    inventory = SceneBuilder::getDefaultInventory(idx, inventory.score);

#ifdef DEBUG_REMOVALS
    std::cout << std::format(
        "Respawning player with context {} at index {}", marker.stateIdx, idx)
              << std::endl;
#endif

    if (marker.rebindCamera) scene.cameraAnchorIdx = idx;

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
        e.damage,
        e.originatorStateIdx);
}

void GameRulesEngine::operator()(ScriptTriggeredGameEvent e)
{
#ifdef DEBUG_REMOVALS

    std::cout << std::format(
        "{}: scriptTriggered(idx = {}, script = {})",
        scene.tick,
        e.targetEntityIdx,
        static_cast<int>(e.scriptId))
              << std::endl;

#endif

    auto& thing = scene.things[e.targetEntityIdx];
    const auto position =
        Position { thing.hitbox.getPosition()
                   + thing.direction * thing.hitbox.getRadius() * 2.f };

    if (e.scriptId == ScriptId::TriggerSound || !e.sound.empty())
    {
        eventQueue->emplace<SoundTriggeredAudioEvent>(
            e.sound, e.sourceType, thing.stateIdx, thing.hitbox.getPosition());
    }

    switch (e.scriptId)
    {
        using enum ScriptId;
    case FireFlare:
        fireFlare(position, Direction { thing.direction }, thing.stateIdx);
        break;
    case FirePellets:
        firePellets(
            position,
            Direction { thing.direction },
            e.targetEntityIdx,
            thing.stateIdx);
        break;
    case FireBullet:
        fireBullet(
            position,
            Direction { thing.direction },
            e.targetEntityIdx,
            thing.stateIdx);
        break;
    case FireLaserDart:
        fireLaserDart(position, Direction { thing.direction }, thing.stateIdx);
        break;

    case FireRocket:
        fireRocket(position, Direction { thing.direction }, thing.stateIdx);
        break;

    case FireRay:
        fireBallista(
            position,
            Direction { thing.direction },
            e.targetEntityIdx,
            thing.stateIdx);
        break;

    case ReleaseTrigger:
        scene.playerStates[thing.stateIdx].input.stopShooting();
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
        }
    }
}

void GameRulesEngine::handlePlayer(Entity& thing, std::size_t idx)
{
    auto& state = scene.playerStates[thing.stateIdx];

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

    const auto isAllowedToFire =
        state.inventory.animationContext.animationStateId
        == AnimationStateId::Idle;
    const auto isAllowedToSwapWeapons =
        isAllowedToFire
        || state.inventory.animationContext.animationStateId
               == AnimationStateId::Recovery;

    // No weapon interaction allowed when weapon is not in idle
    if (isAllowedToFire && state.input.isShooting()
        && canFireActiveWeapon(state.inventory))
    {
        eventQueue->emplace<PlayerFiredAnimationEvent>(idx);
    }
    else if (isAllowedToSwapWeapons)
    {
        if (state.input.shouldSwapToPreviousWeapon())
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
    MarkerDeadPlayer& marker, const std::size_t idx)
{
    auto& input = scene.playerStates[marker.stateIdx].input;
    if (input.isShooting())
    {
        input.stopShooting();
        eventQueue->emplace<PlayerRespawnedGameEvent>(idx);
    }
}

void GameRulesEngine::handleItemRespawner(
    MarkerItemRespawner& marker, const std::size_t idx, const float deltaTime)
{
    marker.timeout -= deltaTime;
    if (marker.timeout <= ITEM_SPAWN_EFFECT_TIMEOUT
        && !marker.spawnEffectEmitted)
    {
        eventQueue->emplace<EffectSpawnedGameEvent>(
            EntityType::EffectSpawn, marker.position);
        marker.spawnEffectEmitted = true;
    }
    else if (marker.timeout <= 0.f)
    {
        eventQueue->emplace<PickupSpawnedGameEvent>(
            marker.pickupType, marker.position, idx);
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
        eventQueue->emplace<PickablePickedUpGameEvent>(pickupId);

        if (inventory.ownerIdx == scene.cameraAnchorIdx)
            eventQueue->emplace<PickablePickedUpAudioEvent>(
                pickup.typeId, entity.stateIdx);
    }
}

bool GameRulesEngine::canFireActiveWeapon(PlayerInventory& inventory) noexcept
{
    return getAmmoCountForActiveWeapon(inventory)
           >= getMinimumAmmoNeededToFireWeapon(inventory.activeWeaponType);
}

void GameRulesEngine::swapToPreviousWeapon(
    PlayerInventory& inventory, EntityIndexType idx)
{
    inventory.activeWeaponType = weaponIndexToType(getPrevToggledBit(
        weaponTypeToIndex(inventory.activeWeaponType),
        inventory.acquiredWeapons));
    eventQueue->emplace<WeaponSwappedAnimationEvent>(idx);
}

void GameRulesEngine::swapToNextWeapon(
    PlayerInventory& inventory, EntityIndexType idx)
{
    inventory.activeWeaponType = weaponIndexToType(getNextToggledBit(
        weaponTypeToIndex(inventory.activeWeaponType),
        inventory.acquiredWeapons));
    eventQueue->emplace<WeaponSwappedAnimationEvent>(idx);
}

bool GameRulesEngine::give(
    Entity& entity, PlayerInventory& inventory, EntityType pickupType)
{
    using enum EntityType;

    const auto& def = ENTITY_PROPERTIES.at(pickupType);

    switch (pickupType)
    {
    case PickupHealth:
        if (entity.health >= MAX_HEALTH) return false;
        entity.health =
            std::clamp(entity.health + def.healthAmount, 0, MAX_HEALTH);
        break;
    case PickupArmor:
        if (entity.armor >= MAX_ARMOR) return false;
        entity.armor = std::clamp(entity.armor + def.armorAmount, 0, MAX_ARMOR);
        break;
    case PickupMegaHealth:
    case PickupMegaArmor:
        entity.health =
            std::clamp(entity.health + def.healthAmount, 0, MAX_UPPED_HEALTH);
        entity.armor =
            std::clamp(entity.armor + def.armorAmount, 0, MAX_UPPED_ARMOR);
        break;
    case PickupBullets:
    case PickupShells:
    case PickupEnergy:
    case PickupRockets: {
        const auto ammoIndex = ammoPickupToAmmoIndex(pickupType);
        if (inventory.ammo[ammoIndex] == AMMO_LIMIT[ammoIndex]) return false;
        inventory.ammo[ammoIndex] = std::clamp(
            inventory.ammo[ammoIndex] + def.ammoAmount,
            0,
            AMMO_LIMIT[ammoIndex]);
    }
    break;
    case PickupShotgun:
    case PickupTrishot:
    case PickupCrossbow:
    case PickupLauncher:
    case PickupBallista: {
        const auto ammoIndex = ammoTypeToAmmoIndex(def.ammoType);
        const auto index = weaponPickupToIndex(pickupType);
        if (inventory.acquiredWeapons[index]) return false;
        give(entity, inventory, ammoTypeToPickupType(def.ammoType));
        inventory.acquiredWeapons[index] = true;
        return false;
    }
    default:
        return false;
    }

    return true;
}

void GameRulesEngine::damage(
    Entity& thing,
    std::size_t idx,
    int damage,
    PlayerStateIndexType originatorStateIdx)
{
    if (thing.health <= 0) return;

    // TODO: max 2/3 of armor above 100?
    // Max 1/3 of the damage can be absorbed by the armor
    int amountAbsorbedByArmor = std::min(thing.armor, damage / 3);
    thing.armor -= amountAbsorbedByArmor;
    damage -= amountAbsorbedByArmor;

    thing.health -= damage;
    if (idx == scene.cameraAnchorIdx) scene.redOverlayIntensity = 128.f;

    if (thing.health <= 0)
    {
        auto& inventory = scene.playerStates[originatorStateIdx].inventory;
        // decrement score if killing myself
        inventory.score += idx == inventory.ownerIdx ? -1 : 1;
        removeEntity(idx);
    }
}

#pragma region Helpers

void GameRulesEngine::removeEntity(std::size_t index)
{
    const auto thing = scene.things[index];
    const auto& DEF = ENTITY_PROPERTIES.at(thing.typeId);
    const bool playerWasDestroyed = thing.typeId == EntityType::Player;

#ifdef DEBUG_REMOVALS

    std::cout << std::format(
        "{}: removeEntity({}) with type {}",
        scene.tick,
        index,
        static_cast<int>(thing.typeId))
              << std::endl;

#endif

    EntityIndexType effectIdx;
    if (DEF.debrisEffectType != EntityType::None)
    {
        effectIdx = scene.things.emplaceBack(SceneBuilder::createEffect(
            DEF.debrisEffectType,
            Position { thing.hitbox.getPosition() },
            scene.tick));
    }

    if (playerWasDestroyed)
    {
        auto& state = scene.playerStates[thing.stateIdx];
        state.inventory.ownerIdx = std::numeric_limits<EntityIndexType>::max();
        state.input.stopShooting();

        scene.things[effectIdx].direction = thing.direction;

        bool rebindCamera = scene.cameraAnchorIdx == index;
        scene.markers.emplaceBack(MarkerDeadPlayer {
            .rebindCamera = rebindCamera, .stateIdx = thing.stateIdx });

        if (rebindCamera) scene.cameraAnchorIdx = effectIdx;
    }

    scene.spatialIndex.removeFromLookup(index, thing.hitbox);
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
    PlayerStateIndexType inventoryIdx)
{
    auto& inventory = scene.playerStates[inventoryIdx].inventory;
    --inventory.ammo[ammoTypeToAmmoIndex(
        ENTITY_PROPERTIES.at(EntityType::WeaponFlaregun).ammoType)];

    eventQueue->emplace<ProjectileCreatedGameEvent>(
        EntityType::ProjectileFlare,
        position.value,
        direction.value,
        inventoryIdx);
}

void GameRulesEngine::firePellets(
    const Position& position,
    const Direction& direction,
    EntityIndexType playerIdx,
    PlayerStateIndexType inventoryIdx)
{
    auto& inventory = scene.playerStates[inventoryIdx].inventory;
    --inventory.ammo[ammoTypeToAmmoIndex(AmmoType::Shells)];

    forEachDirection(
        direction.value,
        getPerpendicular(direction.value) * SHOTGUN_SPREAD,
        SHOTGUN_PELLET_AMOUNT,
        [&](const sf::Vector2f& hitscanDir)
        {
            auto hit = hitscanner.hitscan(
                position, Direction { hitscanDir }, playerIdx);
            eventQueue->emplace<HitscanProjectileFiredGameEvent>(
                hit, SHELL_DAMAGE, inventoryIdx);
        });
}

void GameRulesEngine::fireBullet(
    const Position& position,
    const Direction& direction,
    EntityIndexType playerIdx,
    PlayerStateIndexType inventoryIdx)
{
    auto& inventory = scene.playerStates[inventoryIdx].inventory;
    --inventory.ammo[ammoTypeToAmmoIndex(AmmoType::Bullets)];

    auto hit = hitscanner.hitscan(position, direction, playerIdx);
    eventQueue->emplace<HitscanProjectileFiredGameEvent>(
        hit, TRISHOT_BULLET_DAMAGE, inventoryIdx);
}

void GameRulesEngine::fireLaserDart(
    const Position& position,
    const Direction& direction,
    PlayerStateIndexType inventoryIdx)
{
    auto& inventory = scene.playerStates[inventoryIdx].inventory;
    --inventory.ammo[ammoTypeToAmmoIndex(
        ENTITY_PROPERTIES.at(EntityType::WeaponCrossbow).ammoType)];

    eventQueue->emplace<ProjectileCreatedGameEvent>(
        EntityType::ProjectileLaserDart,
        position.value,
        direction.value,
        inventoryIdx);
}

void GameRulesEngine::fireRocket(
    const Position& position,
    const Direction& direction,
    PlayerStateIndexType inventoryIdx)
{
    auto& inventory = scene.playerStates[inventoryIdx].inventory;
    --inventory.ammo[ammoTypeToAmmoIndex(AmmoType::Rockets)];

    eventQueue->emplace<ProjectileCreatedGameEvent>(
        EntityType::ProjectileRocket,
        position.value,
        direction.value,
        inventoryIdx);
}

void GameRulesEngine::fireBallista(
    const Position& position,
    const Direction& direction,
    EntityIndexType playerIdx,
    PlayerStateIndexType inventoryIdx)
{
    auto& inventory = scene.playerStates[inventoryIdx].inventory;
    --inventory.ammo[ammoTypeToAmmoIndex(AmmoType::Bullets)];

    auto hit = hitscanner.hitscan(position, direction, playerIdx);
    eventQueue->emplace<HitscanProjectileFiredGameEvent>(
        hit, 200, inventoryIdx);

    auto dirToImpact = hit.impactPosition - position.value;
    const float dirSize = dgm::Math::getSize(dirToImpact);
    auto unitDirToImpact = dirToImpact / dirSize;
    for (float t = 8.f; t < dirSize; t += 8.f)
    {
        scene.things.emplaceBack(SceneBuilder::createEffect(
            EntityType::EffectRailDecal,
            Position { position.value + unitDirToImpact * t },
            scene.tick));
    }
}

#pragma endregion

#pragma once

#include "core/Scene.hpp"
#include "core/Types.hpp"
#include "events/GameRuleEvents.hpp"
#include <DGM/DGM.hpp>
#include <events/EventQueue.hpp>
#include <utils/Hitscanner.hpp>

import Memory;

struct GiveResult
{
    bool given = true;
    bool removePickup = true;
    bool playSound = true;
};

class GameRulesEngine final
{
public:
    [[nodiscard]] GameRulesEngine(
        Scene& scene, mem::Rc<EventQueue> eventQueue) noexcept
        : scene(scene), eventQueue(eventQueue), hitscanner(scene)
    {
    }

public: // Must visit on all related events
    void operator()(const PickablePickedUpGameEvent&);
    void operator()(const ProjectileCreatedGameEvent&);
    void operator()(const ProjectileDestroyedGameEvent&);
    void operator()(const EntityDestroyedGameEvent&);
    void operator()(const PlayerRespawnedGameEvent&);
    void operator()(const EffectSpawnedGameEvent&);
    void operator()(const PickupSpawnedGameEvent&);
    void operator()(const HitscanProjectileFiredGameEvent&);
    void operator()(const ScriptTriggeredGameEvent&);

public:
    void update(const float deltaTime);

    void deleteMarkedObjects();

    [[nodiscard]] sf::Vector2f getBestSpawnPosition() const noexcept;

    [[nodiscard]] sf::Vector2f
    getBestSpawnDirection(const sf::Vector2f& spawnPosition) const noexcept;

private:
    void handlePlayer(Entity& thing, std::size_t id, const float dt);

    void handleDeadPlayer(MarkerDeadPlayer& thing, const std::size_t id);

    void handleItemRespawner(
        MarkerItemRespawner& thing,
        const std::size_t idx,
        const float deltaTime);

    void handleGrabbedPickable(
        Entity& entity,
        PlayerInventory& inventory,
        Entity pickup,
        std::size_t pickupId);

    [[nodiscard]] bool
    canFireActiveWeapon(PlayerInventory& inventory) const noexcept;

    void selectPreviousWeapon(PlayerInventory& inventory);

    void selectNextWeapon(PlayerInventory& inventory);

    void swapToSelectedWeapon(PlayerState& state, EntityIndexType idx);

    void swapToLastWeapon(PlayerInventory& inventory, EntityIndexType idx);

    [[nodiscard]] GiveResult
    give(Entity& entity, PlayerInventory& inventory, EntityType pickupId);

    [[nodiscard]] int computeProjectileDamage(
        const int baseDamage,
        const bool isExplosive,
        const dgm::Circle& entityHitbox,
        const dgm::Circle& explosionHitbox) noexcept;

    void damage(
        Entity& thing,
        std::size_t thingIndex,
        int damage,
        PlayerStateIndexType originatorStateIdx);

    auto getOverlapCandidates(const dgm::Circle& hitbox)
    {
        return scene.spatialIndex.getOverlapCandidates(hitbox)
               | std::views::transform(
                   [&](std::size_t id) -> std::pair<std::size_t, Entity&> {
                       return { id, scene.things[id] };
                   });
    }

    void removeEntity(std::size_t index);

private: // Scripts API
    void firePellets(
        int amount,
        int damage,
        const Position& position,
        const Direction& direction,
        EntityIndexType playerIdx,
        PlayerStateIndexType inventoryIdx);

    void fireBullet(
        int damage,
        const Position& position,
        const Direction& direction,
        EntityIndexType playerIdx,
        PlayerStateIndexType inventoryIdx);

    void fireProjectile(
        EntityType projectileType,
        const Position& position,
        const Direction& direction,
        PlayerStateIndexType inventoryIdx);

    void fireRay(
        int damage,
        const Position& position,
        const Direction& direction,
        EntityIndexType playerIdx,
        PlayerStateIndexType inventoryIdx);

private:
    Scene& scene;
    mem::Rc<EventQueue> eventQueue;
    Hitscanner hitscanner;
    std::vector<EntityIndexType> indicesToRemove;
};

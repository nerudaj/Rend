#pragma once

#include "core/Scene.hpp"
#include "core/Types.hpp"
#include "events/GameRuleEvents.hpp"
#include <DGM/DGM.hpp>
#include <Memory.hpp>
#include <engine/GameModeSpecificRulesEngineInterface.hpp>
#include <events/EventQueue.hpp>
#include <utils/GiveResult.hpp>
#include <utils/Hitscanner.hpp>

class [[nodiscard]] GameRulesEngine final
{
public:
    [[nodiscard]] GameRulesEngine(
        Scene& scene,
        mem::Rc<EventQueue> eventQueue,
        GameModeSpecificRulesEngineInterface& modeSpecificRules,
        const std::vector<std::string>& playerNames) noexcept
        : scene(scene)
        , eventQueue(eventQueue)
        , modeSpecificRules(modeSpecificRules)
        , playerNames(playerNames)
        , hitscanner(scene)
    {
    }

public: // Must visit on all related events
    void operator()(const PickablePickedUpGameEvent&);
    void operator()(const ProjectileCreatedGameEvent&);
    void operator()(const ProjectileDestroyedGameEvent);
    void operator()(const EntityDestroyedGameEvent&);
    void operator()(const PlayerRespawnedGameEvent&);
    void operator()(const EffectSpawnedGameEvent&);
    void operator()(const PickupSpawnedGameEvent&);
    void operator()(const HitscanProjectileFiredGameEvent&);
    void operator()(const ScriptTriggeredGameEvent&);
    void operator()(const PlayerKilledThemselvesGameEvent&);
    void operator()(const PlayerKilledPlayerGameEvent&);
    void operator()(const WeaponPickedUpGameEvent&);
    void operator()(const FlagDeliveredGameEvent&);

public:
    void update(const float deltaTime);

    void deleteMarkedObjects();

    [[nodiscard]] const Spawn& getBestSpawn(Team team) const noexcept;

private:
    void handlePlayer(Entity& thing, const EntityIndexType idx, const float dt);

    void
    handleDeadPlayer(MarkerDeadPlayer& thing, const MarkerIndexType markerIdx);

    void handleItemRespawner(
        MarkerItemRespawner& thing,
        const MarkerIndexType markerIdx,
        const float deltaTime);

    void handleDamageOverTime(
        MarkerDamageOverTime& marker,
        const MarkerIndexType markerIdx,
        const float dt);

    void handleGrabbedPickable(
        Entity& player,
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
        EntityIndexType thingIndex,
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

    void removeEntity(EntityIndexType index);

    void removeMarker(MarkerIndexType index);

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
    GameModeSpecificRulesEngineInterface& modeSpecificRules;
    std::vector<std::string> playerNames;
    Hitscanner hitscanner;
    std::vector<EntityIndexType> entityIndicesToRemove;
    std::vector<MarkerIndexType> markerIndicesToRemove;
};

#pragma once

#include "core/Scene.hpp"
#include "events/GameRuleEvents.hpp"
#include <DGM/DGM.hpp>

class GameRulesEngine final
{
public:
    [[nodiscard]] GameRulesEngine(Scene& scene) noexcept : scene(scene) {}

public: // Must visit on all related events
    void operator()(const PickablePickedUpGameEvent&);
    void operator()(const ProjectileCreatedGameEvent&);
    void operator()(const ProjectileDestroyedGameEvent&);
    void operator()(const EntityDestroyedGameEvent&);
    void operator()(const PlayerRespawnedGameEvent&);
    void operator()(const EffectSpawnedGameEvent&);
    void operator()(const PickupSpawnedGameEvent&);

public:
    void update(const float deltaTime);

private:
    void handlePlayer(Entity& thing, std::size_t id, const float deltaTime);

    void handleDeadPlayer(MarkerDeadPlayer& thing, std::size_t id);

    void handleItemRespawner(
        MarkerItemRespawner& thing, std::size_t idx, const float deltaTime);

    void handleGrabbedPickable(
        Entity& entity,
        PlayerInventory& inventory,
        Entity pickup,
        std::size_t pickupId);

    /// <returns>True if thing was succesfully given</returns>
    bool give(Entity& entity, PlayerInventory& inventory, EntityType pickupId);

    void damage(Entity& thing, std::size_t thingIndex, int damage);

    auto getOverlapCandidates(const dgm::Circle& hitbox)
    {
        return scene.spatialIndex.getOverlapCandidates(hitbox)
               | std::views::transform(
                   [&](std::size_t id) -> std::pair<std::size_t, Entity&> {
                       return { id, scene.things[id] };
                   });
    }

    void removeEntity(std::size_t index);

    [[nodiscard]] sf::Vector2f getBestSpawnPosition() const noexcept;

    [[nodiscard]] sf::Vector2f
    getBestSpawnDirection(const sf::Vector2f& spawnPosition) const noexcept;

private:
    Scene& scene;
};

#pragma once

#include <core/Scene.hpp>
#include <events/AnimationEvents.hpp>

class AnimationEngine
{
public:
    [[nodiscard]] AnimationEngine(Scene& scene) noexcept : scene(scene) {}

public: // Must visit on all related events
    void operator()(const SetStateAnimationEvent&);
    void operator()(const PlayerFiredAnimationEvent&);
    void operator()(const WeaponSwappedAnimationEvent&);

public:
    void update(const float deltaTime);

private:
    void handleUpdate(
        AnimationContext& context, EntityType entityType, std::size_t idx);

    void handleTransition(
        AnimationContext& context,
        EntityType entityType,
        std::size_t entityIdx,
        const AnimationState& oldState);

    [[nodiscard]] constexpr auto&
    getInventory(this auto&& self, EntityIndexType playerIdx) noexcept
    {
        return self.scene.playerStates[self.scene.things[playerIdx].stateId]
            .inventory;
    }

    void
    setWeaponAnimationState(EntityIndexType playerIdx, AnimationStateId state);

    void updateSpriteId(
        AnimationContext& context,
        const AnimationState& state,
        EntityIndexType idx);

private:
    Scene& scene;
};

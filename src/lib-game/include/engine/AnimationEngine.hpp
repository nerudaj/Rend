#pragma once

#include <core/Scene.hpp>
#include <events/AnimationEvents.hpp>
#include <events/EventQueue.hpp>

import Memory;

class AnimationEngine
{
public:
    [[nodiscard]] AnimationEngine(
        Scene& scene, mem::Rc<EventQueue> eventQueue) noexcept
        : scene(scene), eventQueue(eventQueue)
    {
    }

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
        return self.scene.playerStates[self.scene.things[playerIdx].stateIdx]
            .inventory;
    }

    void
    setWeaponAnimationState(EntityIndexType playerIdx, AnimationStateId state);

    void setEntityAnimationState(EntityIndexType idx, AnimationStateId state);

    void updateSpriteId(
        AnimationContext& context,
        const AnimationState& state,
        EntityIndexType idx);

private:
    Scene& scene;
    mem::Rc<EventQueue> eventQueue;
};

#pragma once

#include <core/EntityDefinitionTypes.hpp>
#include <core/Scene.hpp>
#include <engine/EngineCommonBase.hpp>
#include <events/AnimationEvents.hpp>
#include <events/EventQueue.hpp>

import Memory;

class AnimationEngine final : public EngineCommonBase
{
public:
    [[nodiscard]] AnimationEngine(
        Scene& scene, mem::Rc<EventQueue> eventQueue) noexcept
        : EngineCommonBase(scene), eventQueue(eventQueue)
    {
    }

    AnimationEngine(AnimationEngine&&) = default;
    AnimationEngine(const AnimationEngine&) = delete;

public: // Must visit on all related events
    void operator()(const PlayerIsRunningAnimationEvent&);
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

    void
    setWeaponAnimationState(EntityIndexType playerIdx, AnimationStateId state);

    void setEntityAnimationState(EntityIndexType idx, AnimationStateId state);

    void updateSpriteId(
        AnimationContext& context,
        const AnimationState& state,
        EntityIndexType idx);

private:
    mem::Rc<EventQueue> eventQueue;
};

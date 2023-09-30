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

    void updateSpriteId(AnimationContext& context, const AnimationState& state);

private:
    Scene& scene;
};

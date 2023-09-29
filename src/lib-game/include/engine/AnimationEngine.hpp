#pragma once

#include <core/AnimationDefinitions.hpp>
#include <core/Scene.hpp>
#include <events/AnimationEvents.hpp>

class AnimationEngine
{
public:
    [[nodiscard]] AnimationEngine(Scene& scene) noexcept : scene(scene) {}

public: // Must visit on all related events
    void operator()(const SetStateAnimationEvent&);

public:
    void update(const float deltaTime);

private:
    void handleTransition(
        Entity& entity, std::size_t entityIdx, const AnimationState& oldState);

    void updateSpriteId(Entity& entity, const AnimationState& state);

private:
    Scene& scene;
};

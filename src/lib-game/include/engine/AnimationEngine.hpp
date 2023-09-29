#pragma once

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
    void handleMarker(Entity& entity, std::size_t index);

private:
    Scene& scene;
};

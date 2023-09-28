#pragma once

#include "core/Scene.hpp"
#include "events/PhysicsEvents.hpp"
#include <DGM/DGM.hpp>

class PhysicsEngine final
{
public:
    [[nodiscard]] PhysicsEngine(Scene& scene) noexcept : scene(scene) {}

public: // Must visit on all related events
    void operator()(const DummyPhysicsEvent&) {}

public: // Visiting on thing types
    void update(const float deltaTime);

private:
    void handlePlayer(Entity& thing, const float deltaTime);
    void handleProjectile(Entity& thing, std::size_t id, float deltaTime);

private:
    Scene& scene;
};

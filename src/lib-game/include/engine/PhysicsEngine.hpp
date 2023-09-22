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
    void update(const dgm::Time& time);

private:
    Scene& scene;
};

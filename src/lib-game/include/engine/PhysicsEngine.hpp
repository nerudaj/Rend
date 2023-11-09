#pragma once

#include "core/Scene.hpp"
#include <DGM/DGM.hpp>
#include <events/EventQueue.hpp>

import Memory;

class PhysicsEngine final
{
public:
    [[nodiscard]] PhysicsEngine(
        Scene& scene, mem::Rc<EventQueue> eventQueue) noexcept
        : scene(scene), eventQueue(eventQueue)
    {
    }

public: // Visiting on thing types
    void update(const float deltaTime);

private:
    void handlePlayer(Entity& thing, const float deltaTime);
    void handleProjectile(Entity& thing, std::size_t id, float deltaTime);

private:
    Scene& scene;
    mem::Rc<EventQueue> eventQueue;
};

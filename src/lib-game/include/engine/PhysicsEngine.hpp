#pragma once

#include "core/Scene.hpp"
#include <DGM/DGM.hpp>
#include <core/EntityDefinitionTypes.hpp>
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
    void handlePlayer(
        const EntityProperties& properties,
        Entity& thing,
        const float deltaTime);

    void handleProjectile(
        const EntityProperties& properties,
        Entity& thing,
        std::size_t id,
        float deltaTime);

private:
    Scene& scene;
    mem::Rc<EventQueue> eventQueue;
};

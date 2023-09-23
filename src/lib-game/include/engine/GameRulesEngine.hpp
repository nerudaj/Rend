#pragma once

#include "core/Scene.hpp"
#include "events/GameRuleEvents.hpp"
#include <DGM/DGM.hpp>

class GameRulesEngine final
{
public:
    [[nodiscard]] GameRulesEngine(Scene& scene) noexcept : scene(scene) {}

public: // Must visit on all related events
    void operator()(const PickablePickedUpGameEvent&);

public:
    void update(const dgm::Time& time);

private:
    void
    handleGrabbedPickable(Entity grabber, Entity pickup, unsigned pickupId);

    /// <returns>True if thing was succesfully given</returns>
    bool give(Entity& thing, EntityType pickupId);

private:
    Scene& scene;
};

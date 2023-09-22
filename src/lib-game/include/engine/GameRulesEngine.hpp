#pragma once

#include "core/Scene.hpp"
#include "events/GameRuleEvents.hpp"
#include <DGM/DGM.hpp>

class GameRulesEngine final
{
public:
    [[nodiscard]] GameRulesEngine(Scene& scene) noexcept : scene(scene) {}

public: // Must visit on all related events
    void operator()(const DummyGameEvent&) {}

public:
    void update(const dgm::Time& time);

private:
    void give(Player& actor, PickupId pickup);

private:
    Scene& scene;
};

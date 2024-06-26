#pragma once

#include <core/Scene.hpp>

class [[nodiscard]] EngineCommonBase
{
public:
    explicit EngineCommonBase(Scene& scene) noexcept : scene(scene) {}

    virtual ~EngineCommonBase() = default;

public:
    [[nodiscard]] constexpr auto&
    getInventory(this auto&& self, EntityIndexType playerIdx) noexcept
    {
        return self.scene.playerStates[self.scene.things[playerIdx].stateIdx]
            .inventory;
    }

protected:
    Scene& scene;
};
#pragma once

#include <core/Types.hpp>
#include <cstdlib>
#include <variant>

struct [[nodiscard]] PlayerIsRunningAnimationEvent
{
    EntityIndexType playerIdx;

    PlayerIsRunningAnimationEvent(EntityIndexType playerIdx) noexcept
        : playerIdx(playerIdx)
    {
    }
};

struct [[nodiscard]] PlayerFiredAnimationEvent
{
    EntityIndexType playerIdx;

    PlayerFiredAnimationEvent(EntityIndexType playerIdx) noexcept
        : playerIdx(playerIdx)
    {
    }
};

struct [[nodiscard]] WeaponSwappedAnimationEvent
{
    EntityIndexType playerIdx;
    AnimationStateId animationId;

    WeaponSwappedAnimationEvent(
        EntityIndexType playerIdx, AnimationStateId animationId) noexcept
        : playerIdx(playerIdx), animationId(animationId)
    {
    }
};

using AnimationEvent = std::variant<
    PlayerIsRunningAnimationEvent,
    PlayerFiredAnimationEvent,
    WeaponSwappedAnimationEvent>;
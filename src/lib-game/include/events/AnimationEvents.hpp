#pragma once

#include <core/Types.hpp>
#include <cstdlib>
#include <variant>

struct SetStateAnimationEvent
{
    EntityIndexType thingsId;
    int stateId;
};

struct PlayerFiredAnimationEvent
{
    EntityIndexType playerIdx;

    PlayerFiredAnimationEvent(EntityIndexType playerIdx) : playerIdx(playerIdx)
    {
    }
};

struct WeaponSwappedAnimationEvent
{
    EntityIndexType playerIdx;

    WeaponSwappedAnimationEvent(EntityIndexType playerIdx)
        : playerIdx(playerIdx)
    {
    }
};

using AnimationEvent =
    std::variant<SetStateAnimationEvent, PlayerFiredAnimationEvent>;
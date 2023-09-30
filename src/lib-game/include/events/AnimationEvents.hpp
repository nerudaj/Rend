#pragma once

#include <cstdlib>
#include <variant>

struct SetStateAnimationEvent
{
    std::size_t thingsId;
    int stateId;
};

struct PlayerFiredAnimationEvent
{
    std::size_t playerIdx;

    PlayerFiredAnimationEvent(std::size_t playerIdx) : playerIdx(playerIdx) {}
};

using AnimationEvent =
    std::variant<SetStateAnimationEvent, PlayerFiredAnimationEvent>;
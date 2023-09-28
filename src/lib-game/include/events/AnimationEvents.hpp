#pragma once

#include <cstdlib>
#include <variant>

struct SetStateAnimationEvent
{
    std::size_t thingsId;
    int stateId;
};

using AnimationEvent = std::variant<SetStateAnimationEvent>;
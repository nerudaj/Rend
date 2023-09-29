#pragma once

#include <core/Enums.hpp>
#include <map>
#include <vector>

using AnimationClip = std::vector<SpriteId>;

struct AnimationState
{
    AnimationClip clip;
    std::size_t updateFrequency; // how many frames pass before next update
    AnimationStateId transition;
};

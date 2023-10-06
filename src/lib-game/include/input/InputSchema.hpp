#pragma once

#include <nlohmann/json.hpp>

struct InputSchema
{
    bool shooting = false;
    bool prevWeapon = false;
    bool nextWeapon = false;
    bool lastWeapon = false;
    float thrust = 0.f;
    float sidewardThrust = 0.f;
    float steer = 0.f;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    InputSchema,
    shooting,
    prevWeapon,
    nextWeapon,
    lastWeapon,
    thrust,
    sidewardThrust,
    steer);

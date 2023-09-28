#pragma once

#include <nlohmann/json.hpp>

struct InputSchema
{
    bool shooting = false;
    float thrust = 0.f;
    float sidewardThrust = 0.f;
    float steer = 0.f;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    InputSchema, shooting, thrust, sidewardThrust, steer);

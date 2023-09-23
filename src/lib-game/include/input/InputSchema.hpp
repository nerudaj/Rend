#pragma once

#include <nlohmann/json.hpp>

struct InputSchema
{
    float thrust = 0.f;
    float sidewardThrust = 0.f;
    float steer = 0.f;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InputSchema, thrust, sidewardThrust, steer);

#pragma once

#include <nlohmann/json.hpp>

struct InputOptions
{
    float mouseSensitivity = 25.f;
    float turnSpeed = 1.f;
    float gamepadDeadzone = 0.05f;
    float cursorSensitivity_v2 = 250.f;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    InputOptions,
    mouseSensitivity,
    turnSpeed,
    gamepadDeadzone,
    cursorSensitivity_v2);

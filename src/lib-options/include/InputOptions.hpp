#pragma once

#include <nlohmann/json.hpp>

struct InputOptions
{
    float mouseSensitivity = 25.f;
    float turnSpeed = 1.f;
    float gamepadDeadzone = 0.05f;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    InputOptions, mouseSensitivity, turnSpeed, gamepadDeadzone);

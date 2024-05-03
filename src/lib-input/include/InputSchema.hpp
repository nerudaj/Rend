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

void to_json(nlohmann::json& j, const InputSchema& schema);

void from_json(const nlohmann::json& json, InputSchema& schema);

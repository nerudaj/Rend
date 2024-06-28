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

    constexpr std::partial_ordering
    operator<=>(const InputSchema&) const = default;

    [[nodiscard]] constexpr bool isEmpty() const noexcept
    {
        return *this == InputSchema {};
    }
};

void to_json(nlohmann::json& j, const InputSchema& schema);

void from_json(const nlohmann::json& json, InputSchema& schema);

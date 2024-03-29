module;

#include <nlohmann/json.hpp>

export module InputOptions;

export
{
    struct InputOptions
    {
        float mouseSensitivity = 25.f;
        float turnSpeed = 1.f;
        float gamepadDeadzone = 0.05f;
        bool autoswapOnPickup = false;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        InputOptions,
        mouseSensitivity,
        turnSpeed,
        gamepadDeadzone,
        autoswapOnPickup);
}

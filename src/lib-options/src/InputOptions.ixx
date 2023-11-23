module;

#include <nlohmann/json.hpp>

export module InputOptions;

export
{
    struct InputOptions
    {
        float mouseSensitivity = 1.f;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InputOptions, mouseSensitivity);
}

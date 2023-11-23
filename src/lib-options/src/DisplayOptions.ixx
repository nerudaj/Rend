module;

#include <nlohmann/json.hpp>

export module DisplayOptions;

export
{
    struct Resolution
    {
        unsigned width;
        unsigned height;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Resolution, width, height);

    struct DisplayOptions
    {
        Resolution resolution = { 1280u, 720u };
        bool isFullscreen = false;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        DisplayOptions, resolution, isFullscreen);
}

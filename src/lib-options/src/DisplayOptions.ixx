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
#ifdef _DEBUG
        bool isFullscreen = false;
#else
        bool isFullscreen = true;
#endif
        bool useDitheredShadows = false;
        bool showFps = false;
        float fov = 0.66f;
        bool hideHud = false;
        bool showCrosshair = true;
        float hudScale = 1.f;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        DisplayOptions,
        resolution,
        isFullscreen,
        useDitheredShadows,
        showFps,
        hideHud,
        showCrosshair,
        hudScale);
}

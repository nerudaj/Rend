module;

#include <nlohmann/json.hpp>

export module AudioOptions;

export
{

    struct AudioOptions
    {
        float musicVolume = 50.f;
        float soundVolume = 50.f;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AudioOptions, musicVolume, soundVolume);
}

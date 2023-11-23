module;

#include <nlohmann/json.hpp>

export module AppOptions;

import AudioOptions;
import DisplayOptions;
import InputOptions;
import CmdParameters;

export
{

    struct AppOptions
    {
        AudioOptions audio;
        InputOptions input;
        DisplayOptions display;
        CmdParameters cmdSettings;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AppOptions, audio, input, display);
}

#pragma once

#include "AudioOptions.hpp"
#include "CmdParameters.hpp"
#include "DisplayOptions.hpp"
#include "InputOptions.hpp"
#include "UserOptions.hpp"
#include <nlohmann/json.hpp>

struct AppOptions
{
    AudioOptions audio;
    InputOptions input;
    DisplayOptions display;
    UserOptions player;
    CmdParameters cmdSettings;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AppOptions, audio, input, player, display);

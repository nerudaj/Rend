#pragma once

#include "AudioOptions.hpp"
#include "CmdParameters.hpp"
#include "DisplayOptions.hpp"
#include "InputOptions.hpp"
#include "LobbySettings.hpp"
#include "NetOptions.hpp"
#include "UserOptions.hpp"
#include <nlohmann/json.hpp>

struct AppOptions
{
    AudioOptions audio;
    InputOptions input;
    DisplayOptions display;
    UserOptions player;
    CmdParameters cmdSettings;
    NetOptions network;
    LobbySettings lastUsedLobbySettings;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    AppOptions, audio, input, player, display, network);

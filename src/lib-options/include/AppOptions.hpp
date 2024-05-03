#pragma once

#include <nlohmann/json.hpp>
#include "AudioOptions.hpp"
#include "InputOptions.hpp"
#include "DisplayOptions.hpp"
#include "CmdParameters.hpp"

struct AppOptions
{
	AudioOptions audio;
	InputOptions input;
	DisplayOptions display;
	CmdParameters cmdSettings;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AppOptions, audio, input, display);

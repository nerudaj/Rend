#pragma once

#include <nlohmann/json.hpp>

struct AudioOptions
{
	float musicVolume = 50.f;
	float soundVolume = 50.f;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AudioOptions, musicVolume, soundVolume);

#pragma once

#include <map>
#include <string>

enum class [[nodiscard]] AiPersonality
{
    Default,
    Tank,     // ignores hurt
    Flash,    // no targetting timer,
    Speartip, // rushes with shotgun
};

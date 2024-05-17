#pragma once

#include "InputSchema.hpp"
#include "NetworkTypes.hpp"
#include <nlohmann/json.hpp>

struct [[nodiscard]] InputData
{
    PlayerIdxType clientId;
    size_t tick;
    InputSchema input;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InputData, clientId, tick, input);

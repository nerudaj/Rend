#pragma once

#include "InputSchema.hpp"
#include <nlohmann/json.hpp>
#include "NetworkTypes.hpp"

struct [[nodiscard]] InputData
{
	PlayerIdType clientId;
	size_t tick;
	InputSchema input;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InputData, clientId, tick, input);

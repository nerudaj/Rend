#pragma once

#include <nlohmann/json.hpp>
#include <string>

struct [[nodiscard]] ClientData final
{
	bool active = true;
	std::string name;
	// TODO: settings like swap on pickup
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ClientData, active, name);

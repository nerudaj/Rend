#pragma once

#include <vector>
#include <string>
#include <json.hpp>

struct TriggerActionDefinition
{
	unsigned id;
	std::string name;
	std::vector<std::string> params;
};

void from_json(
	const nlohmann::json& json,
	TriggerActionDefinition& definition);
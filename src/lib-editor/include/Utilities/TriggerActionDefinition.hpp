#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

struct TriggerActionDefinition
{
    unsigned id;
    std::string name;
    std::vector<std::string> params;
};

void from_json(const nlohmann::json& json, TriggerActionDefinition& definition);
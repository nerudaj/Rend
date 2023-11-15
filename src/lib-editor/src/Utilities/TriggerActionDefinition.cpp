#include "include/Utilities/TriggerActionDefinition.hpp"

#include <format>

void from_json(const nlohmann::json& json, TriggerActionDefinition& definition)
{
	try
	{
		json["id"].get_to(definition.id);
	}
	catch (std::exception& e)
	{
		throw std::runtime_error(
			std::format(
				"When parsing 'id' property:\n{}", e.what()));
	}

	try
	{
		json["name"].get_to(definition.name);
	}
	catch (std::exception& e)
	{
		throw std::runtime_error(
			std::format(
				"When parsing 'name' property:\n{}", e.what()));
	}

	try
	{
		json["params"].get_to(definition.params);
	}
	catch (std::exception& e)
	{
		throw std::runtime_error(
			std::format(
				"When parsing 'params' property:\n{}", e.what()));
	}

	// TODO: default param values
}

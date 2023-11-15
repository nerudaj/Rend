#pragma once

#include "include/Interfaces/ToolPropertyInterface.hpp"
#include "include/Utilities/TriggerActionDefinition.hpp"
#include "include/Utilities/GC.hpp"

#include <TGUI/TGUI.hpp>
#include <LevelD.hpp>

#include <climits>
#include <map>

class TriggerToolProperty : public ToolPropertyInterface
{
public:
	TriggerToolProperty(
		GC<std::map<unsigned, TriggerActionDefinition>> actionDefinitions,
		LevelD::Trigger data,
		std::size_t id) noexcept
		: actionDefinitions(actionDefinitions)
		, data(data)
		, id(id)
	{}

public:
	void fillEditDialog(
		tgui::Panel::Ptr& panel,
		FormValidatorToken& formValidatorToken) override;

public:
	GC<std::map<unsigned, TriggerActionDefinition>> actionDefinitions;
	LevelD::Trigger data;
	std::size_t id;

private:
	std::vector<std::string> getActionNames() const;
};

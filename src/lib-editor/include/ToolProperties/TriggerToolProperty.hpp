#pragma once

#include "Interfaces/ToolPropertyInterface.hpp"
#include "Utilities/TriggerActionDefinition.hpp"
#include <LevelD.hpp>
#include <TGUI/TGUI.hpp>
#include <climits>
#include <map>

import Memory;

class TriggerToolProperty : public ToolPropertyInterface
{
public:
    TriggerToolProperty(
        mem::Rc<std::map<unsigned, TriggerActionDefinition>> actionDefinitions,
        LevelD::Trigger data,
        std::size_t id) noexcept
        : actionDefinitions(actionDefinitions), data(data), id(id)
    {
    }

public:
    void fillEditDialog(
        tgui::Panel::Ptr& panel,
        FormValidatorToken& formValidatorToken) override;

public:
    mem::Rc<std::map<unsigned, TriggerActionDefinition>> actionDefinitions;
    LevelD::Trigger data;
    std::size_t id;

private:
    std::vector<std::string> getActionNames() const;
};

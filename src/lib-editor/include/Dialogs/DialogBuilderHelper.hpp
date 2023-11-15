#pragma once

#include "Dialogs/FormValidatorToken.hpp"
#include <TGUI/TGUI.hpp>
#include <cstdint>
#include <string>

namespace DialogBuilderHelper
{
    void addOption(
        tgui::ScrollablePanel::Ptr& target,
        const std::string& label,
        const std::string& tooltip,
        bool& data,
        unsigned ypos,
        bool enabled = true);

    void addOption(
        tgui::Container::Ptr target,
        FormValidatorToken& token, // TODO: can be removed?
        const std::string& label,
        const std::string& tooltip,
        uint32_t& data,
        unsigned ypos,
        bool enabled = true,
        bool tag = false);

    void addOption(
        tgui::ScrollablePanel::Ptr& target,
        FormValidatorToken& token,
        const std::string& label,
        const std::string& tooltip,
        uint16_t& data,
        unsigned ypos,
        bool enabled = true);

    void addOption(
        tgui::ScrollablePanel::Ptr& target,
        const std::string& label,
        const std::string& tooltip,
        std::string& data,
        unsigned ypos,
        bool enabled = true);

    void addComboOption(
        tgui::Container::Ptr target,
        const std::string& label,
        const std::string& tooltip,
        const std::vector<std::string>& values,
        std::function<void(std::size_t)> onChangeCallback,
        std::size_t initialIndex,
        unsigned ypos);

    unsigned int getRowHeight();
}; // namespace DialogBuilderHelper

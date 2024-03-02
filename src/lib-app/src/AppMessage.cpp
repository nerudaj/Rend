#include <optional>
#include <string>

import AppMessage;

bool PopIfNotMainMenu::canDeserializeFrom(const std::string& str)
{
    return str == std::string_view(serialize());
}

std::optional<AppMessage> deserializeAppMessage(const std::string& str)
{
    if (PopIfNotMainMenu::canDeserializeFrom(str)) return PopIfNotMainMenu();

    return std::nullopt;
}
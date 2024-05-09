#include "utils/AppMessage.hpp"

std::optional<AppMessage> deserializeAppMessage(const std::string& str)
{
    if (PopIfNotMainMenu::canDeserializeFrom(str))
        return PopIfNotMainMenu();
    else if (PopIfPause::canDeserializeFrom(str))
        return PopIfPause();
    else if (PopIfNotMapRotationWrapper::canDeserializeFrom(str))
        return PopIfNotMapRotationWrapper();
    return std::nullopt;
}

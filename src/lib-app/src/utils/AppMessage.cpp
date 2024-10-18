#include "utils/AppMessage.hpp"

std::optional<AppMessage> deserializeAppMessage(const std::string& str)
{
    if (PopIfNotMainMenu::canDeserializeFrom(str))
        return PopIfNotMainMenu();
    else if (PopIfPause::canDeserializeFrom(str))
        return PopIfPause();
    else if (PopIfNotMapRotationWrapper::canDeserializeFrom(str))
        return PopIfNotMapRotationWrapper();
    else if (ExceptionGameDisconnected::canDeserializeFrom(str))
        return ExceptionGameDisconnected();
    else if (ExceptionServerOffline::canDeserializeFrom(str))
        return ExceptionServerOffline();
    else if (ExceptionNoMapSelected::canDeserializeFrom(str))
        return ExceptionNoMapSelected();
    else if (ExceptionCtfMapWrongCompat::canDeserializeFrom(str))
        return ExceptionCtfMapWrongCompat();
    return std::nullopt;
}

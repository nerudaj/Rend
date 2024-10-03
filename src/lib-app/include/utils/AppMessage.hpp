#pragma once

#include "app/AppStateMainMenu.hpp"
#include "app/AppStateMapRotationWrapper.hpp"
#include "app/AppStatePaused.hpp"
#include <Configs/Strings.hpp>
#include <CoreTypes.hpp>
#include <optional>
#include <string>
#include <typeinfo>
#include <variant>

template<class This>
struct CanDeserializeFrom
{
    static [[nodiscard]] constexpr bool
    canDeserializeFrom(const std::string& str) noexcept
    {
        return str == std::string_view(This::serialize());
    }

    static [[nodiscard]] constexpr const char* serialize() noexcept
    {
        return typeid(This).name();
    }
};

struct [[nodiscard]] PopIfNotMainMenu final
    : public CanDeserializeFrom<PopIfNotMainMenu>
{
};

struct [[nodiscard]] PopIfPause final : public CanDeserializeFrom<PopIfPause>
{
};

struct [[nodiscard]] PopIfNotMapRotationWrapper final
    : public CanDeserializeFrom<PopIfNotMapRotationWrapper>
{
};

struct [[nodiscard]] ExceptionGameDisconnected final
    : public CanDeserializeFrom<ExceptionGameDisconnected>
{
};

struct [[nodiscard]] ExceptionServerOffline final
    : public CanDeserializeFrom<ExceptionServerOffline>
{
};

struct [[nodiscard]] ExceptionNoMapSelected final
    : public CanDeserializeFrom<ExceptionNoMapSelected>
{
};

using AppMessage = std::variant<
    PopIfNotMainMenu,
    PopIfPause,
    PopIfNotMapRotationWrapper,
    ExceptionGameDisconnected,
    ExceptionServerOffline,
    ExceptionNoMapSelected,

[[nodiscard]] std::optional<AppMessage>
deserializeAppMessage(const std::string& str);

template<class CallerState>
std::optional<std::string>
handleAppMessage(dgm::App& app, const std::string& message)
{
    auto&& msg = deserializeAppMessage(message);
    if (!msg) return std::nullopt;

    std::optional<std::string> result;
    std::visit(
        overloaded {
            [&](PopIfNotMainMenu)
            {
                if (!std::is_same_v<CallerState, AppStateMainMenu>)
                    app.popState(message);
            },
            [&](PopIfNotMapRotationWrapper)
            {
                if (!std::is_same_v<CallerState, AppStateMapRotationWrapper>)
                    app.popState(message);
            },
            [&](PopIfPause)
            {
                if (std::is_same_v<CallerState, AppStatePaused>)
                    app.popState(message);
            },
            [&](ExceptionGameDisconnected)
            {
                if (!std::is_same_v<CallerState, AppStateMainMenu>)
                    app.popState(message);
                else
                    result = Strings::Error::DESYNCED;
            },
            [&](ExceptionServerOffline)
            {
                if (!std::is_same_v<CallerState, AppStateMainMenu>)
                    app.popState(message);
                else
                    result = Strings::Error::SERVER_OFFLINE;
            },
            [&](ExceptionNoMapSelected)
            {
                if (!std::is_same_v<CallerState, AppStateMainMenu>)
                    app.popState(message);
                else
                    result = Strings::Error::NO_MAP_SELECTED;
            } },
        msg.value());
    return result;
}
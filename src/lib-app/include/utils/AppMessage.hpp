#pragma once

#include "app/AppStateMainMenu.hpp"
#include "app/AppStateMapRotationWrapper.hpp"
#include "app/AppStatePaused.hpp"
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

using AppMessage =
    std::variant<PopIfNotMainMenu, PopIfPause, PopIfNotMapRotationWrapper>;

[[nodiscard]] std::optional<AppMessage>
deserializeAppMessage(const std::string& str);

template<class CallerState>
void handleAppMessage(dgm::App& app, const std::string& message)
{
    auto&& msg = deserializeAppMessage(message);
    if (!msg) return;
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
            } },
        msg.value());
}
#pragma once

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

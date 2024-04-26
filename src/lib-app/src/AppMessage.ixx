module;

#include <optional>
#include <string>
#include <typeinfo>
#include <variant>

export module AppMessage;

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

export struct [[nodiscard]] PopIfNotMainMenu final
    : public CanDeserializeFrom<PopIfNotMainMenu>
{
};

export struct [[nodiscard]] PopIfPause final
    : public CanDeserializeFrom<PopIfPause>
{
};

export struct [[nodiscard]] PopIfNotMapRotationWrapper final
    : public CanDeserializeFrom<PopIfNotMapRotationWrapper>
{
};

export using AppMessage =
    std::variant<PopIfNotMainMenu, PopIfPause, PopIfNotMapRotationWrapper>;

export [[nodiscard]] std::optional<AppMessage>
deserializeAppMessage(const std::string& str)
{
    if (PopIfNotMainMenu::canDeserializeFrom(str))
        return PopIfNotMainMenu();
    else if (PopIfPause::canDeserializeFrom(str))
        return PopIfPause();
    else if (PopIfNotMapRotationWrapper::canDeserializeFrom(str))
        return PopIfNotMapRotationWrapper();
    return std::nullopt;
}

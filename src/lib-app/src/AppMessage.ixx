module;

#include <optional>
#include <string>
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
};

export struct [[nodiscard]] PopIfNotMainMenu final
    : public CanDeserializeFrom<PopIfNotMainMenu>
{
    static [[nodiscard]] constexpr const char* serialize() noexcept
    {
        return "pop if not main menu";
    }
};

export struct [[nodiscard]] PopIfPause final
    : public CanDeserializeFrom<PopIfPause>
{
    static [[nodiscard]] constexpr const char* serialize() noexcept
    {
        return "pop if pause";
    }
};

export using AppMessage = std::variant<PopIfNotMainMenu, PopIfPause>;

export [[nodiscard]] std::optional<AppMessage>
deserializeAppMessage(const std::string& str)
{
    if (PopIfNotMainMenu::canDeserializeFrom(str))
        return PopIfNotMainMenu();
    else if (PopIfPause::canDeserializeFrom(str))
        return PopIfPause();
    return std::nullopt;
}

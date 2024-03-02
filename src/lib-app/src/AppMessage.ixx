module;

#include <optional>
#include <string>
#include <variant>

export module AppMessage;

export struct PopIfNotMainMenu
{
    static [[nodiscard]] bool canDeserializeFrom(const std::string& str);

    static [[nodiscard]] constexpr const char* serialize() noexcept
    {
        return "pop if not main menu";
    }
};

export using AppMessage = std::variant<PopIfNotMainMenu>;

export [[nodiscard]] std::optional<AppMessage>
deserializeAppMessage(const std::string& str);
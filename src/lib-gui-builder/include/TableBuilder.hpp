#pragma once

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <optional>
#include <string>
#include <vector>

namespace priv
{
    class [[nodiscard]] TableBuilder final
    {
    public:
        constexpr TableBuilder() noexcept {}

        explicit constexpr TableBuilder(
            const std::vector<std::string>& headingCells)
            : heading(headingCells)
        {
        }

        void addRow(const std::vector<std::string>& cells);

        [[nodiscard]] tgui::Panel::Ptr build();

    private:
        std::optional<std::vector<std::string>> heading;
        std::vector<std::vector<std::string>> rowsOfCells;
    };
} // namespace priv

class [[nodiscard]] TableBuilder final
{
public:
    constexpr priv::TableBuilder withNoHeading() const noexcept
    {
        return priv::TableBuilder();
    }

    constexpr priv::TableBuilder
    withHeading(const std::vector<std::string>& cells)
    {
        return priv::TableBuilder(cells);
    }
};

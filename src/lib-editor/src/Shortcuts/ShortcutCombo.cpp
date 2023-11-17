#include "Shortcuts/ShortcutCombo.hpp"
#include <numeric>
#include <vector>

std::strong_ordering intCompare(int a, int b)
{
    if (a < b)
        return std::strong_ordering::less;
    else if (a > b)
        return std::strong_ordering::greater;
    return std::strong_ordering::equal;
}

std::strong_ordering
ShortcutCombo::operator<=>(const ShortcutCombo& other) const
{
    if (ctrlRequired != other.ctrlRequired)
        return intCompare(ctrlRequired, other.ctrlRequired);
    else if (shiftRequired != other.shiftRequired)
        return intCompare(shiftRequired, other.shiftRequired);
    return intCompare(key, other.key);
}

namespace std
{
    string to_string(const sf::Keyboard::Key key)
    {
        if (sf::Keyboard::A <= key && key <= sf::Keyboard::Z)
            return string(1, 'A' + key);
        else
            return "key" + to_string(static_cast<int>(key));
    }

    string to_string(const ShortcutCombo& combo)
    {
        std::vector<std::string> words;
        if (combo.ctrlRequired) words.push_back("Ctrl");
        if (combo.shiftRequired) words.push_back("Shift");
        words.push_back(std::to_string(combo.key));

        return accumulate(
            ++words.begin(),
            words.end(),
            *words.begin(),
            [](const std::string& acc, const std::string& word) -> std::string
            { return acc + "+" + word; });
    }
} // namespace std

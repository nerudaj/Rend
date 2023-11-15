#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <compare>
#include <string>

struct ShortcutCombo {
	bool ctrlRequired = false;
	bool shiftRequired = false;
	sf::Keyboard::Key key;

	std::strong_ordering operator<=>(const ShortcutCombo& other) const;
};

namespace std
{
	string to_string(const ShortcutCombo& combo);
}

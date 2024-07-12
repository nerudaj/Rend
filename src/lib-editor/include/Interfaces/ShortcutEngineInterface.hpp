#pragma once

#include "Shortcuts/ShortcutCombo.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <functional>

class ShortcutEngineInterface
{
public:
    virtual ~ShortcutEngineInterface() = default;

public:
    virtual void handleEvent(sf::Event& event) = 0;

    virtual void registerShortcut(
        const std::string& groupName,
        const ShortcutCombo& combo,
        std::function<void(void)> callback) = 0;

    virtual void releaseSpecialKeys() {};

    // Removes all shortcuts registered with matching group name
    virtual void unregisterShortcutGroup(const std::string& groupName) = 0;
};

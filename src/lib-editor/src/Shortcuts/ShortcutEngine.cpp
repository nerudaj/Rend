#include "Shortcuts/ShortcutEngine.hpp"
#include <cassert>

void ShortcutEngine::evaluateShortcut(sf::Keyboard::Key key)
{
    if (ignoreShortcuts())
    {
        return;
    }

    ShortcutCombo combo = { .ctrlRequired = ctrlIsPressed,
                            .shiftRequired = shiftIsPressed,
                            .key = key };

    if (shortcuts.contains(combo)) shortcuts.at(combo)();
}

void ShortcutEngine::handleEvent(sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::LControl)
            ctrlIsPressed = true;
        else if (event.key.code == sf::Keyboard::LShift)
            shiftIsPressed = true;
        else
            evaluateShortcut(event.key.code);
    }
    else if (event.type == sf::Event::KeyReleased)
    {
        if (event.key.code == sf::Keyboard::LControl)
            ctrlIsPressed = false;
        else if (event.key.code == sf::Keyboard::LShift)
            shiftIsPressed = false;
    }
}

void ShortcutEngine::releaseSpecialKeys()
{
    ctrlIsPressed = false;
    shiftIsPressed = false;
}

void ShortcutEngine::registerShortcut(
    const std::string& groupName,
    const ShortcutCombo& combo,
    std::function<void(void)> callback)
{
    assert(!shortcuts.contains(combo));
    shortcuts[combo] = callback;
    groupToShortcuts[groupName].push_back(combo);
}

void ShortcutEngine::unregisterShortcutGroup(const std::string& groupName)
{
    if (!groupToShortcuts.contains(groupName)) return;

    for (auto&& combo : groupToShortcuts.at(groupName))
    {
        shortcuts.erase(combo);
    }
}

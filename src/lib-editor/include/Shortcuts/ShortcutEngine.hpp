#pragma once

#include "include/Interfaces/ShortcutEngineInterface.hpp"
#include "include/Shortcuts/ShortcutCombo.hpp"
#include <map>

class ShortcutEngine final : public ShortcutEngineInterface
{
public:
    ShortcutEngine(std::function<bool(void)> ignoreShortcuts)
        : ignoreShortcuts(ignoreShortcuts)
    {
    }

public:
    void evaluateShortcut(sf::Keyboard::Key key);

    void handleEvent(sf::Event& event) override;

    void registerShortcut(
        const std::string& groupName,
        const ShortcutCombo& combo,
        std::function<void(void)> callback) override;

    void unregisterShortcutGroup(const std::string& groupName) override;

protected:
    typedef std::function<void(void)> Callback;

protected:
    std::function<bool(void)> ignoreShortcuts;
    bool ctrlIsPressed = false;
    bool shiftIsPressed = false;
    std::map<ShortcutCombo, Callback> shortcuts;
    std::map<std::string, std::vector<ShortcutCombo>> groupToShortcuts;
};

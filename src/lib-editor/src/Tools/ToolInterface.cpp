#include "Interfaces/ToolInterface.hpp"

static inline const std::string CTX_MENU_NAME = "Tool";

void ToolInterface::buildCtxMenu(tgui::MenuBar::Ptr& menu)
{
    destroyCtxMenu(menu);
    shortcutEngine->unregisterShortcutGroup(CTX_MENU_NAME);

    if (!menu->getMenuEnabled(CTX_MENU_NAME))
    {
        menu->addMenu(CTX_MENU_NAME);
    }
    buildCtxMenuInternal(menu);
}

void ToolInterface::addCtxMenuItem(
    tgui::MenuBar::Ptr& menu,
    const std::string& label,
    std::function<void(void)> callback,
    sf::Keyboard::Key key)
{
    menu->addMenuItem(CTX_MENU_NAME, label);
    ctxMenuSignalHandlers.push_back(
        menu->connectMenuItem(CTX_MENU_NAME, label, callback));
    shortcutEngine->registerShortcut(
        CTX_MENU_NAME, { false, true, key }, callback);
}

void ToolInterface::destroyCtxMenu(tgui::MenuBar::Ptr& menu)
{
    ctxMenuSignalHandlers.clear();
    menu->removeMenuItems(CTX_MENU_NAME);
}

sf::Vector2i Helper::minVector(const sf::Vector2i& a, const sf::Vector2i& b)
{
    return sf::Vector2i(std::min(a.x, b.x), std::min(a.y, b.y));
}

sf::Vector2i Helper::maxVector(const sf::Vector2i& a, const sf::Vector2i& b)
{
    return sf::Vector2i(std::max(a.x, b.x), std::max(a.y, b.y));
}

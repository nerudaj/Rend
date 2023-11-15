#include "include/Tools/SidebarUserTrigger.hpp"

void SidebarUserTrigger::buildSidebarInternal(tgui::Group::Ptr& sidebar)
{
    const float SIDEBAR_WIDTH = sidebar->getSize().x;
    const float OFFSET = 10.f;
    const float BUTTON_SIZE = SIDEBAR_WIDTH - 2 * OFFSET;

    auto cbtn = tgui::Button::create("Circular\nTrigger");
    cbtn->setTextSize(0);
    cbtn->setRenderer(gui->theme.getRenderer("Button"));
    cbtn->setSize(BUTTON_SIZE, BUTTON_SIZE);
    cbtn->setPosition(OFFSET, OFFSET);
    cbtn->connect("clicked", [this] { changePen(PenType::Circle); });
    cbtn->getRenderer()->setOpacity(
        penType != PenType::Circle ? 0.25f : 1.f); // highlight
    sidebar->add(cbtn);

    auto rbtn = tgui::Button::create("Rectangular\nTrigger");
    rbtn->setTextSize(0);
    rbtn->setRenderer(gui->theme.getRenderer("Button"));
    rbtn->setSize(BUTTON_SIZE, BUTTON_SIZE);
    rbtn->setPosition(OFFSET, 2 * OFFSET + BUTTON_SIZE);
    rbtn->connect("clicked", [this] { changePen(PenType::Rectangle); });
    rbtn->getRenderer()->setOpacity(
        penType == PenType::Circle ? 0.25f : 1.f); // highlight
    sidebar->add(rbtn);
}

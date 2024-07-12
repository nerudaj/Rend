#include "Interfaces/ToolPropertyWithImageInterface.hpp"
#include <WidgetBuilder.hpp>

void ToolPropertyWithImageInterface::fillEditDialog(tgui::Panel::Ptr& panel)
{
    const float IMAGE_SIZE = panel->getSize().x / 4.f;
    constexpr unsigned VERTICAL_OFFSET = 20;

    auto imagePanel = WidgetBuilder::createPanel({ IMAGE_SIZE, IMAGE_SIZE });
    imagePanel->setPosition(
        (panel->getSize().x - IMAGE_SIZE) / 2.f, VERTICAL_OFFSET);
    imagePanel->getRenderer()->setTextureBackground(previewImage);
    panel->add(imagePanel);

    auto contentPanel = tgui::ScrollablePanel::create(
        { "100%",
          ("100% - " + std::to_string(IMAGE_SIZE + VERTICAL_OFFSET * 2))
              .c_str() });
    contentPanel->setPosition({ 0, IMAGE_SIZE + VERTICAL_OFFSET * 2 });
    contentPanel->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
    panel->add(contentPanel);

    fillEditDialogInternal(contentPanel);
}

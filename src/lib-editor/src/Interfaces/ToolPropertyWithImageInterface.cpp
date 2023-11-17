#include "Interfaces/ToolPropertyWithImageInterface.hpp"

void ToolPropertyWithImageInterface::fillEditDialog(
    tgui::Panel::Ptr& panel, FormValidatorToken& formValidatorToken)
{
    const float IMAGE_SIZE = panel->getSize().x / 4.f;
    constexpr unsigned VERTICAL_OFFSET = 20;

    auto imagePanel = tgui::Panel::create();
    imagePanel->setSize(IMAGE_SIZE, IMAGE_SIZE);
    imagePanel->setPosition(
        (panel->getSize().x - IMAGE_SIZE) / 2.f, VERTICAL_OFFSET);
    imagePanel->getRenderer()->setTextureBackground(previewImage);
    panel->add(imagePanel);

    auto contentPanel = tgui::Panel::create();
    contentPanel->setSize(
        { "100%",
          ("100% - " + std::to_string(IMAGE_SIZE + VERTICAL_OFFSET * 2))
              .c_str() });
    contentPanel->setPosition({ 0, IMAGE_SIZE + VERTICAL_OFFSET * 2 });
    panel->add(contentPanel);

    fillEditDialogInternal(contentPanel, formValidatorToken);
}

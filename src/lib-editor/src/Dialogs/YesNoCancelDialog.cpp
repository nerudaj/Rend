#include "Dialogs/YesNoCancelDialog.hpp"
#include "Globals.hpp"
#include <array>
#include <utility>

void YesNoCancelDialog::open(
    const std::string title,
    const std::string& text,
    std::function<void(UserChoice)> completedCallback)
{
    auto cancelDialog = [this, completedCallback]
    {
        gui->closeModal(DIALOG_ID);
        completedCallback(UserChoice::Cancelled);
    };

    auto modal = gui->createNewChildWindow(title);
    modal->setSize("20%", "20%");
    modal->setPosition("40%", "40%");
    modal->setPositionLocked(true);
    modal->onEscapeKeyPress(cancelDialog);
    modal->onClose(cancelDialog);
    gui->addModal(modal, DIALOG_ID);

    auto label = tgui::Label::create(text);
    label->setSize({ "98%", "80%" });
    label->setPosition({ "1%", "1%" });
    label->setTextSize(Sizers::GetMenuBarTextHeight());
    modal->add(label);

    const tgui::Layout2d BUTTON_SIZE = { "30%", "25%" };
    const std::string BUTTON_Y_OFFSET = "72%";
    typedef std::pair<std::string, UserChoice> ButtonType;
    const std::array<ButtonType, 3> BUTTONS = {
        ButtonType { "Yes", UserChoice::Confirmed },
        ButtonType { "No", UserChoice::Denied },
        ButtonType { "Cancel", UserChoice::Cancelled }
    };
    float xOffset = 2.25f;

    for (auto&& [labelText, choice] : BUTTONS)
    {
        auto btn = tgui::Button::create(labelText);
        btn->setSize(BUTTON_SIZE);
        btn->setPosition(
            (std::to_string(xOffset) + "%").c_str(), BUTTON_Y_OFFSET.c_str());
        btn->onClick(
            [this, choice, completedCallback]
            {
                gui->closeModal(DIALOG_ID);
                completedCallback(choice);
            });
        modal->add(btn);
        xOffset += 32.25f;
    }
}
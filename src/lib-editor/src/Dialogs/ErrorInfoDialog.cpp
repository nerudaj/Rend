#include "include/Dialogs/ErrorInfoDialog.hpp"
#include "include/Globals.hpp"

void ErrorInfoDialog::open(const std::string& text)
{
    auto modal = gui->createNewChildWindow("Error");
    modal->setSize("30%", "20%");
    modal->setPosition("35%", "40%");
    modal->setPositionLocked(true);

    modal->connect("Closed", [&] { gui->closeModal(DIALOG_ID); });
    gui->addModal(modal, DIALOG_ID);

    auto label = tgui::Label::create(text);
    label->setSize({ "98%", "98%" });
    label->setPosition({ "1%", "1%" });
    label->setTextSize(Sizers::GetMenuBarTextHeight());
    modal->add(label);
}

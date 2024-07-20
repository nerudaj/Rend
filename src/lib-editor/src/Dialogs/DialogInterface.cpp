#include "Dialogs/DialogInterface.hpp"
#include "Globals.hpp"
#include <Configs/Strings.hpp>

DialogInterface::DialogInterface(
    mem::Rc<Gui> gui,
    const std::string& dialogId,
    const std::string& dialogTitle)
    : gui(gui), DIALOG_ID(dialogId), DIALOG_TITLE(dialogTitle)
{
}

void DialogInterface::open(std::function<void()> confirmCallback)
{
    if (isOpen()) return;
    buildLayout(confirmCallback);
}

void DialogInterface::buildLayout(std::function<void()> confirmCallback)
{
    auto modal = gui->createNewChildWindow(DIALOG_TITLE);
    modal->setSize("30%", "50%");
    modal->setPosition("35%", "25%");
    modal->onEscapeKeyPress([this](auto) { close(); });
    modal->onClose([this] { close(); });
    gui->add(modal, DIALOG_ID);

    auto panel = tgui::Panel::create({ "90%", "85%" });
    panel->setPosition({ "5%", "5%" });
    panel->setRenderer(gui->theme->getRenderer("Panel"));
    modal->add(panel);

    buildBottomButtons(modal, confirmCallback);
    buildLayoutImpl(panel);
}

void DialogInterface::buildBottomButtons(
    tgui::ChildWindow::Ptr modal, std::function<void()> confirmCallback)
{
    auto btn = tgui::Button::create(Strings::Dialog::SUBMIT_OK);
    btn->setSize("20%", "8%");
    btn->setPosition("56%", "90%");
    btn->onClick(
        [this, confirmCallback]
        {
            auto validationResult = validateBeforeConfirmation();
            if (!validationResult)
            {
                throw std::runtime_error(validationResult.error());
            }
            else
            {
                confirmCallback();
                close();
            }
        });
    modal->add(btn);

    btn = tgui::Button::create(Strings::Dialog::SUBMIT_CANCEL);
    btn->setSize("20%", "8%");
    btn->setPosition("78%", "90%");
    btn->onClick([this] { close(); });
    modal->add(btn);
}

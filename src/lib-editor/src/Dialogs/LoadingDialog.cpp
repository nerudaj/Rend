#pragma once

#include <Configs/Sizers.hpp>
#include <Configs/Strings.hpp>
#include <Dialogs/LoadingDialog.hpp>

void LoadingDialog::open(const std::string& text)
{
    auto modal = gui->createNewChildWindow(Strings::Dialog::Title::LOADING);
    modal->setSize("50%", "20%");
    modal->setPosition("25%", "40%");
    modal->setPositionLocked(true);
    gui->add(modal, DIALOG_ID);

    auto label = tgui::Label::create(text);
    label->setSize({ "98%", "98%" });
    label->setPosition({ "1%", "1%" });
    label->setTextSize(Sizers::GetMenuBarTextHeight());
    modal->add(label);
}

void LoadingDialog::close()
{
    gui->closeModal(DIALOG_ID);
}

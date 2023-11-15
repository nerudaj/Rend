#pragma once

#include "Gui.hpp"
#include "Interfaces/DialogInterfaces.hpp"

class YesNoCancelDialog : public YesNoCancelDialogInterface
{
    mem::Rc<Gui> gui;
    const std::string DIALOG_ID = "YesNoCancelDialog";

public:
    virtual void open(
        const std::string title,
        const std::string& text,
        std::function<void(UserChoice)> completedCallback) override;

    YesNoCancelDialog(mem::Rc<Gui> gui) : gui(gui) {};
};

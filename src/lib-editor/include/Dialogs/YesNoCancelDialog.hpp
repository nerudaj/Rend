#pragma once

#include "Interfaces/DialogInterfaces.hpp"

import Gui;

class [[nodiscard]] YesNoCancelDialog final : public YesNoCancelDialogInterface
{
    mem::Rc<Gui> gui;
    const std::string DIALOG_ID = "YesNoCancelDialog";

public:
    virtual void open(
        const std::string title,
        const std::string& text,
        std::function<void(UserChoice)> completedCallback) override;

    explicit YesNoCancelDialog(mem::Rc<Gui> gui) : gui(gui) {};
};

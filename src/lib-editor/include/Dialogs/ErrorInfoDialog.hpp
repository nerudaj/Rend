#pragma once

#include "Gui.hpp"
#include "Interfaces/DialogInterfaces.hpp"

import Memory;

class ErrorInfoDialog final : public ErrorInfoDialogInterface
{
public:
    ErrorInfoDialog(mem::Rc<Gui> gui) noexcept : gui(gui) {}

public:
    virtual void open(const std::string& text) override;

private:
    mem::Rc<Gui> gui;
    constexpr static inline const char* DIALOG_ID = "ErrorInfoDialog";
};

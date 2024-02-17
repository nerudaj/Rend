#pragma once

#include "Gui.hpp"
#include "Interfaces/DialogInterfaces.hpp"

import Memory;

class [[nodiscard]] LoadingDialog final
{
public:
    LoadingDialog(mem::Rc<Gui> gui) noexcept : gui(gui) {}

public:
    void open(const std::string& text);
    void close();

private:
    mem::Rc<Gui> gui;
    constexpr static inline const char* DIALOG_ID = "LoadingDialog";
};

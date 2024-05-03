#pragma once

#include "DialogBase.hpp"

import Gui;

class ResizeDialog final : public DialogInterface
{
private:
    virtual void customOpenCode() override {}

public:
    [[nodiscard]] unsigned getLevelWidth() const;

    [[nodiscard]] unsigned getLevelHeight() const;

    [[nodiscard]] bool isTranslationDisabled() const;

    ResizeDialog(mem::Rc<Gui> gui);
    ResizeDialog(const ResizeDialog&) = delete;
    ResizeDialog(ResizeDialog&&) = delete;
};

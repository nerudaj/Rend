#pragma once

#include "Dialogs/DialogBase.hpp"

class [[nodiscard]] SaveLevelDialog final : public DialogInterface
{
public:
    SaveLevelDialog(mem::Rc<Gui> gui);

public:
    std::string getLevelName() const;

protected:
    void customOpenCode() override {}
};

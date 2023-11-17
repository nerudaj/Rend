#pragma once

#include <Dialogs/DialogBase.hpp>

class [[nodiscard]] LoadLevelDialog final : public DialogInterface
{
public:
    LoadLevelDialog(mem::Rc<Gui> gui, const std::filesystem::path& rootDir);

    [[nodiscard]] std::string getLevelName() const;

protected:
    void customOpenCode() override;

private:
    std::string pickedValue;
};

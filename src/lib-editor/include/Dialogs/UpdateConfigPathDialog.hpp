#pragma once

#include "Dialogs/DialogBase.hpp"
#include "Interfaces/FileApiInterface.hpp"

class UpdateConfigPathDialog final : public DialogInterface
{
public:
    UpdateConfigPathDialog(mem::Rc<Gui> gui, mem::Rc<FileApiInterface> fileApi);

public:
    [[nodiscard]] std::string getConfigPath() const;

private:
    virtual void customOpenCode() override;
};
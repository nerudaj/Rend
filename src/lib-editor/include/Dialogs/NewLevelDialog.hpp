#pragma once

#include "DialogBase.hpp"
#include "Interfaces/FileApiInterface.hpp"
#include <DGM/dgm.hpp>
#include <optional>

import Memory;

/**
 *  \brief Modal window for creating new level
 *
 *  User should be able to define initial level width, height
 *  and he is also required to provide path to config file.
 *
 *  Programmer first has to open the modal window. If the selection is
 *  made and modal is confirmed, associated callback is fired. Then the
 *  getter functions can be used. Modal is closed automatically.
 */
class NewLevelDialog final : public DialogInterface
{
public:
    NewLevelDialog(mem::Rc<Gui> gui, mem::Rc<FileApiInterface> fileApi);

public:
    [[nodiscard]] unsigned getLevelWidth() const;

    [[nodiscard]] unsigned getLevelHeight() const;

private:
    const std::optional<std::string>& configPath;

private:
    virtual void customOpenCode() override;
};

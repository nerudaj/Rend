#pragma once

#include "Gui.hpp"
#include "Interfaces/DialogInterfaces.hpp"
#include "Interfaces/FileApiInterface.hpp"
#include "Launcher/PlaytestLauncherOptions.hpp"

class PlaytestSettingsDialog final : public PlaytestSettingsDialogInterface
{
public:
    PlaytestSettingsDialog(
        mem::Rc<PlaytestLauncherOptions> options,
        mem::Rc<Gui> gui,
        mem::Rc<FileApiInterface> fileApi);

public:
    [[nodiscard]] std::filesystem::path getBinaryPath() const override;

    [[nodiscard]] std::string getLaunchParameters() const override;

    [[nodiscard]] std::filesystem::path getWorkingDirPath() const override;

private:
    virtual void customOpenCode() override {}
};

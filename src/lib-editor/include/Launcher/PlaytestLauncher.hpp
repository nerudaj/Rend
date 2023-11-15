#pragma once

#include "Interfaces/DialogInterfaces.hpp"
#include "Interfaces/PlaytestLauncherInterface.hpp"
#include "Interfaces/ProcessCreatorInterface.hpp"
#include "Interfaces/ShortcutEngineInterface.hpp"
#include "Launcher/PlaytestLauncherOptions.hpp"

class PlaytestLauncher final : public PlaytestLauncherInterface
{
public:
    PlaytestLauncher(
        mem::Rc<PlaytestLauncherOptions> options,
        mem::Rc<ShortcutEngineInterface> shortcutEngine,
        mem::Rc<ProcessCreatorInterface> processCreator,
        mem::Rc<PlaytestSettingsDialogInterface> dialogPlaytestSettings,
        std::function<std::string()> getCurrentLevelPathCallback) noexcept;

public:
    void buildContextMenu(tgui::MenuBar::Ptr menu) override;

    [[nodiscard]] const std::filesystem::path&
    getBinaryPath() const noexcept override
    {
        return options->pathToBinary;
    }

    [[nodiscard]] const std::string&
    getLaunchParameters() const noexcept override
    {
        return options->parameters;
    }

    [[nodiscard]] const std::filesystem::path&
    getWorkingDirPath() const noexcept override
    {
        return options->workingDirectory;
    }

private:
    void handlePlaytestExecuted();

    void handleConfigureLaunchOptions();

private:
    mem::Rc<PlaytestLauncherOptions> options;
    mem::Rc<ShortcutEngineInterface> shortcutEngine;
    mem::Rc<ProcessCreatorInterface> processCreator;
    mem::Rc<PlaytestSettingsDialogInterface> dialogPlaytestSettings;
    std::function<std::string()> getCurrentLevelPathCallback;
};

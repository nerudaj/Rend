#pragma once

#include "include/Interfaces/DialogInterfaces.hpp"
#include "include/Interfaces/PlaytestLauncherInterface.hpp"
#include "include/Interfaces/ProcessCreatorInterface.hpp"
#include "include/Interfaces/ShortcutEngineInterface.hpp"
#include "include/Launcher/PlaytestLauncherOptions.hpp"
#include "include/Utilities/GC.hpp"

class PlaytestLauncher final : public PlaytestLauncherInterface
{
public:
    PlaytestLauncher(
        GC<PlaytestLauncherOptions> options,
        GC<ShortcutEngineInterface> shortcutEngine,
        GC<ProcessCreatorInterface> processCreator,
        GC<PlaytestSettingsDialogInterface> dialogPlaytestSettings,
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
    GC<PlaytestLauncherOptions> options;
    GC<ShortcutEngineInterface> shortcutEngine;
    GC<ProcessCreatorInterface> processCreator;
    GC<PlaytestSettingsDialogInterface> dialogPlaytestSettings;
    std::function<std::string()> getCurrentLevelPathCallback;
};

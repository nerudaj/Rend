#pragma once

#include <TGUI/TGUI.hpp>
#include <filesystem>
#include <string>

class PlaytestLauncherInterface
{
public:
    virtual ~PlaytestLauncherInterface() = default;

public:
    virtual void buildContextMenu(tgui::MenuBar::Ptr menu) = 0;

    [[nodiscard]] virtual const std::filesystem::path&
    getBinaryPath() const noexcept = 0;

    [[nodiscard]] virtual const std::string&
    getLaunchParameters() const noexcept = 0;

    [[nodiscard]] virtual const std::filesystem::path&
    getWorkingDirPath() const noexcept = 0;
};

#pragma once

#include "include/Interfaces/PlaytestLauncherInterface.hpp"

class NullPlaytestLauncher final : public PlaytestLauncherInterface
{
public:
    void buildContextMenu(tgui::MenuBar::Ptr) override {}

    [[nodiscard]] const std::filesystem::path&
    getBinaryPath() const noexcept override
    {
        return path;
    }

    [[nodiscard]] const std::string& getLaunchParameters() const noexcept
    {
        return str;
    }

    [[nodiscard]] const std::filesystem::path&
    getWorkingDirPath() const noexcept
    {
        return path;
    }

private:
    std::string str = "";
    std::filesystem::path path;
};

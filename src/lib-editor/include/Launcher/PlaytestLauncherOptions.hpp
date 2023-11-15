#pragma once

#include <filesystem>
#include <string>

struct PlaytestLauncherOptions
{
    std::filesystem::path pathToBinary;
    std::string parameters;
    std::filesystem::path workingDirectory;
};

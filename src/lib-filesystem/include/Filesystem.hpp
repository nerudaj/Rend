#pragma once

#include <filesystem>
#include <string>
#include <vector>

class Filesystem
{
public:
    static [[nodiscard]] std::filesystem::path getFullLevelPath(
        const std::filesystem::path& rootDir, const std::string& levelName);

    static [[nodiscard]] std::vector<std::string>
    getLevelNames(const std::filesystem::path& levelsDir);

    static [[nodiscard]] std::filesystem::path
    getLevelsDir(const std::filesystem::path& rootDir);

    static [[nodiscard]] std::filesystem::path
    getEditorConfigPath(const std::filesystem::path& rootDir);
};

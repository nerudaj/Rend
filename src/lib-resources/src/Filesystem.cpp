import Resources;

#include <filesystem>

std::filesystem::path Filesystem::getFullLevelPath(
    const std::filesystem::path& rootDir, const std::string& levelName)
{
    if (levelName.ends_with(".lvd")) return getLevelsDir(rootDir) / levelName;
    return getLevelsDir(rootDir) / (levelName + ".lvd");
}

std::vector<std::string>
Filesystem::getLevelNames(const std::filesystem::path& levelsDir)
{
    std::vector<std::string> result;

    for (auto dirEntry : std::filesystem::directory_iterator(levelsDir))
    {
        if (dirEntry.path().extension().string().contains("lvd"))
            result.push_back(dirEntry.path().stem().string());
    }

    return result;
}

std::filesystem::path
Filesystem::getLevelsDir(const std::filesystem::path& rootDir)
{
    return rootDir / "levels";
}

std::filesystem::path
Filesystem::getEditorConfigPath(const std::filesystem::path& rootDir)
{
    return rootDir / "editor-config.json";
}

#include "Filesystem.hpp"
#include <Windows.h>
#include <codecvt>
#include <filesystem>
#include <locale>
#include <ranges>
#include <shlobj.h>
#include <shlobj_core.h>

std::filesystem::path Filesystem::getFullLevelPath(
    const std::filesystem::path& rootDir,
    const std::string& packName,
    const std::string& levelName)
{
    if (levelName.ends_with(".lvd")) return getLevelsDir(rootDir) / levelName;
    return getLevelsDir(rootDir) / packName / (levelName + ".lvd");
}

std::vector<std::string> Filesystem::getLevelNames(
    const std::filesystem::path& levelsDir, const std::string& packName)
{
    std::vector<std::string> result;

    for (auto&& dirEntry :
         std::filesystem::directory_iterator(levelsDir / packName))
    {
        if (dirEntry.path().extension().string().contains("lvd"))
            result.push_back(dirEntry.path().stem().string());
    }

    return result;
}

std::vector<std::string>
Filesystem::getLevelPackNames(const std::filesystem::path& levelsDir)
{
    return std::views::common(std::filesystem::directory_iterator(levelsDir))
           | std::views::filter(
               [](const std::filesystem::directory_entry& entry)
               { return std::filesystem::is_directory(entry); })
           | std::views::transform(
               [](const std::filesystem::directory_entry& entry)
               { return entry.path().stem().string(); })
           | std::ranges::to<std::vector>();
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

std::filesystem::path Filesystem::getAppdataPath()
{
    PWSTR raw;
    std::wstring result;
    if (SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &raw) == S_OK)
    {
        result = std::wstring(raw);
    }
    CoTaskMemFree(raw);

    // Need to convert wstring to string
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;

    return std::filesystem::path(converter.to_bytes(result));
}

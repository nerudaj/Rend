#include "AppSettings.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

void saveToFile(
    const std::filesystem::path& filepath, const AppSettings& settings)
{
    nlohmann::json jsonSettings;
    jsonSettings["musicVolume"] = settings.musicVolume;
    jsonSettings["soundVolume"] = settings.soundVolume;
    jsonSettings["windowWidth"] = settings.windowWidth;
    jsonSettings["windowHeight"] = settings.windowHeight;
    jsonSettings["fullscreen"] = settings.fullscreen;

    std::ofstream save(filepath);
    save << jsonSettings.dump();
}

AppSettings loadFromFile(const std::filesystem::path& filepath)
{
    std::ifstream load(filepath);
    nlohmann::json jsonSettings;
    load >> jsonSettings;

    auto&& settings = AppSettings {};
    if (jsonSettings.count("musicVolume"))
        settings.musicVolume = jsonSettings["musicVolume"];
    if (jsonSettings.count("soundVolume"))
        settings.soundVolume = jsonSettings["soundVolume"];
    if (jsonSettings.count("windowWidth"))
        settings.windowWidth = jsonSettings["windowWidth"];
    if (jsonSettings.count("windowHeight"))
        settings.windowHeight = jsonSettings["windowHeight"];
    if (jsonSettings.count("fullscreen"))
        settings.fullscreen = jsonSettings["fullscreen"];

    return settings;
}

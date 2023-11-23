#pragma once

#include <filesystem>

struct AppSettings
{
    float soundVolume = 50.f;
    float musicVolume = 50.f;
    unsigned windowWidth = 1280;
    unsigned windowHeight = 720;
    bool fullscreen = false;
};

void saveToFile(
    const std::filesystem::path& filepath, const AppSettings& settings);
AppSettings loadFromFile(const std::filesystem::path& filepath);
#pragma once

#include <filesystem>

/// <summary>
/// Command-line settings
/// </summary>
struct CmdSettings
{
    bool skipMainMenu = false;
    std::filesystem::path resourcesDir = "../resources";
    std::string mapname;
    std::filesystem::path demoFile = "demo.txt";
    bool playDemo = false;
    unsigned playerCount = 4;
    unsigned fraglimit = 15;
};
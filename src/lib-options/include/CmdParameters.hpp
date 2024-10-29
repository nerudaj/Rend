#pragma once

#include <filesystem>
#include <string>

struct CmdParameters
{
    bool skipMainMenu = false;
    std::filesystem::path resourcesDir = "../resources";
#ifdef _DEBUG
    std::filesystem::path demoFile = "demo.txt";
    bool playDemo = false;
#endif
    bool enableDebug = false;
    bool useNullBotBehavior = false;
};

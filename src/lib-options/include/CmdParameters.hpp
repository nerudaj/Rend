#pragma once

#include <filesystem>
#include <string>

struct CmdParameters
{
    bool skipMainMenu = false;
    std::filesystem::path resourcesDir = "../resources";
    std::string mapname;
    std::filesystem::path demoFile = "demo.txt";
    bool playDemo = false;
    bool enableDebug = false;
    size_t maxNpcs = 3;
    unsigned fraglimit = 15;
    bool useNullBotBehavior = false;
};

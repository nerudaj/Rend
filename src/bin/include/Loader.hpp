#pragma once

#include <DGM/dgm.hpp>
#include <LevelD.hpp>
#include <filesystem>

class Loader
{
public:
    static void loadLevel(const std::filesystem::path& path, LevelD& lvd);

    static void loadResources(
        dgm::ResourceManager& resmgr, const std::filesystem::path& rootDir);
};
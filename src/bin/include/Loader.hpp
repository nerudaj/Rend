#pragma once

#include <DGM/dgm.hpp>
#include <filesystem>

class Loader
{
public:
    static void loadResources(
        dgm::ResourceManager& resmgr, const std::filesystem::path& rootDir);
};
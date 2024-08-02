#pragma once

#include "ServerDependencies.hpp"
#include <filesystem>

class [[nodiscard]] ServerMapLoader final : public MapLoaderInterface
{
public:
    explicit constexpr ServerMapLoader(
        const std::filesystem::path& resourcesDir) noexcept
        : resourcesDir(resourcesDir)
    {
    }

public:
    std::expected<std::string, Error> loadMapInBase64(
        const std::string& mapPackName,
        const std::string& mapName) const override;

private:
    static [[nodiscard]] std::string
    makeSureMapNameHasExtension(const std::string& name) noexcept
    {
        return name.ends_with(".lvd") ? name : name + ".lvd";
    }

private:
    std::filesystem::path resourcesDir;
};

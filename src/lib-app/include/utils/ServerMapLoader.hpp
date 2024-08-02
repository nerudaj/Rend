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
    std::filesystem::path resourcesDir;
};

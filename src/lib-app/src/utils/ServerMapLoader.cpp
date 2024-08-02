#include "utils/ServerMapLoader.hpp"
#include "utils/FileLoader.hpp"
#include <base64.hpp>

std::expected<std::string, Error> ServerMapLoader::loadMapInBase64(
    const std::string& mapPackName, const std::string& mapName) const
{
    auto path = resourcesDir / "levels" / mapPackName / mapName;
    if (!std::filesystem::exists(path))
    {
        return std::unexpected(Error("Requested map file does not exist!"));
    }

    auto file = FileLoader::loadFile(path);
    if (!file)
    {
        return file.error();
    }

    return base64::to_base64(file.value());
}
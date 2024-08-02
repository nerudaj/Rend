#include "utils/ServerMapLoader.hpp"
#include "utils/Base16.hpp"
#include "utils/FileLoader.hpp"
#include <base64.hpp>

std::expected<std::string, Error> ServerMapLoader::loadMapInBase64(
    const std::string& mapPackName, const std::string& mapName) const
{
    auto path = resourcesDir / "levels" / mapPackName
                / makeSureMapNameHasExtension(mapName);
    if (!std::filesystem::exists(path))
    {
        return std::unexpected(Error(std::format(
            "Requested map file {} does not exist!", path.string())));
    }

    auto file = FileLoader::loadFile(path);
    if (!file)
    {
        return file.error();
    }

    return Base16::toBase16(file.value());
}
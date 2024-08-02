#include "utils/FileLoader.hpp"

std::expected<std::string, Error>
FileLoader::loadFile(const std::filesystem::path& path) noexcept
{
    auto stream = std::ifstream(path, std::ios::binary);
    if (!stream.good())
        return std::unexpected(
            Error(std::format("Could not open file {}", path.string())));

    stream.seekg(0, std::ios_base::end);
    size_t size = stream.tellg();
    stream.seekg(0, std::ios_base::beg);

    auto buffer = std::string(size, '\0');
    stream.read(buffer.data(), size);

    return buffer;
}
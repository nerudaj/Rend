#pragma once

#include <Error.hpp>
#include <filesystem>
#include <fstream>

class [[nodiscard]] FileLoader final
{
public:
    static std::expected<std::string, Error>
    loadFile(const std::filesystem::path& path) noexcept;
};

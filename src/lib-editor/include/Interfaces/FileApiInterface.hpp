#pragma once

#include <filesystem>
#include <optional>
#include <string>

class FileApiInterface
{
public:
    [[nodiscard]] virtual std::string resolveAppdata() const = 0;

    [[nodiscard]] virtual std::optional<std::string>
    getSaveFileName(const char* filter) const = 0;

    [[nodiscard]] virtual std::optional<std::string>
    getOpenFileName(const char* filter) const = 0;

    [[nodiscard]] virtual std::optional<std::filesystem::path>
    selectFolder() const = 0;

    [[nodiscard]] FileApiInterface() = default;
    FileApiInterface(const FileApiInterface&) = delete;
    virtual ~FileApiInterface() = default;
};

constexpr const char* JSON_FILTER = "JSON Files\0*.json\0Any File\0*.*\0";
constexpr const char* LVLD_FILTER = "LevelD Files\0*.lvd\0Any File\0*.*\0";
constexpr const char* EXES_FILTER = "Executables\0*.exe\0Scripts\0*.bat\0";

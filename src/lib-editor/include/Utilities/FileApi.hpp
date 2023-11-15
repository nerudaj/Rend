#pragma once

#include "include/Interfaces/FileApiInterface.hpp"
#include <filesystem>
#include <optional>
#include <string>

class FileApi final : public FileApiInterface
{
public:
    [[nodiscard]] std::string resolveAppdata() const override;

    [[nodiscard]] std::optional<std::string>
    getSaveFileName(const char* filter) const override;

    [[nodiscard]] std::optional<std::string>
    getOpenFileName(const char* filter) const override;

    [[nodiscard]] std::optional<std::filesystem::path>
    selectFolder() const override;
};

#pragma once

#include "include/Utilities/Error.hpp"
#include <expected>
#include <filesystem>
#include <string>

class ProcessCreatorInterface
{
public:
    virtual ~ProcessCreatorInterface() = default;

public:
    [[nodiscard]] virtual std::expected<ReturnFlag, ErrorMessage> Exec(
        const std::filesystem::path& pathToBinary,
        const std::string& params,
        const std::filesystem::path& workingDirectory) const noexcept = 0;
};

#pragma once

#include "Interfaces/ProcessCreatorInterface.hpp"

class ProcessCreator final : public ProcessCreatorInterface
{
public:
    [[nodiscard]] std::expected<ReturnFlag, ErrorMessage> Exec(
        const std::filesystem::path& pathToBinary,
        const std::string& params,
        const std::filesystem::path& workingDirectory) const noexcept override;
};

#pragma once

#include "logging/LoggerInterface.hpp"
#include <filesystem>
#include <fstream>

class [[nodiscard]] FileLogger final : public LoggerInterface
{
public:
    explicit FileLogger(const std::filesystem::path& logfilePath);

public:
    void logImpl(const std::string& message) override;

private:
    std::ofstream logstream;
};

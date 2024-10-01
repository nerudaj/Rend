#pragma once

#include "LoggerInterface.hpp"
#include <filesystem>
#include <fstream>

class [[nodiscard]] FileLogger final : public LoggerInterface
{
public:
    explicit FileLogger(const std::filesystem::path& logfilePath);

public:
    void logImpl(
        size_t tick, LogSeverity severity, const std::string& message) override;

private:
    std::ofstream logstream;
};

#include "logging/FileLogger.hpp"
#include <print>
#include <ranges>

FileLogger::FileLogger(const std::filesystem::path& logfilePath)
    : logstream(logfilePath)
{
    setLogLevel(LogLevel::All);
    std::println(logstream, "Tick,Severity,Message");
}

static inline std::string replaceAll(
    const std::string& str,
    const std::string_view from,
    const std::string_view to)
{
    return str | std::views::split(from) | std::views::join_with(to)
           | std::ranges::to<std::string>();
}

void FileLogger::logImpl(
    size_t tick, LogSeverity severity, const std::string& message)
{
    std::println(
        logstream,
        "{},{},\"{}\"",
        tick,
        severity == LogSeverity::Debug ? "Debug" : "Error",
        replaceAll(message, "\"", "\"\""));
}

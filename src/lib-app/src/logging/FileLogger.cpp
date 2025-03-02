#include "logging/FileLogger.hpp"
#include <format>
#include <print>
#include <ranges>

FileLogger::FileLogger(const std::filesystem::path& logfilePath)
    : logstream(logfilePath)
{
    setLogLevel(LogLevel::All);
    logstream << "Tick,Severity,Message,Time" << std::endl;
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
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();

    auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
    duration -= hours;
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
    duration -= minutes;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    duration -= seconds;
    auto milliseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    std::string formatted_time = std::format(
        "{:02}:{:02}:{:02}.{:03}",
        hours.count() % 24,
        minutes.count() % 60,
        seconds.count() % 60,
        milliseconds.count());

    logstream << std::format(
        "{},{},\"{}\",{}\n",
        tick,
        severity == LogSeverity::Debug ? "Debug" : "Error",
        replaceAll(message, "\"", "\"\""),
        formatted_time);
}

#pragma once

#include <Error.hpp>
#include <format>
#include <string>

enum class [[nodiscard]] LogSeverity
{
    Debug,
    Error
};

enum class [[nodiscard]] LogLevel
{
    Off,
    All
};

class [[nodiscard]] LoggerInterface
{
public:
    virtual ~LoggerInterface() = default;

public:
    template<class... Args>
    void log(size_t tick, const char* fmt, Args&&... args)
    {
        if (logLevel == LogLevel::Off) return;

        logImpl(
            tick,
            LogSeverity::Debug,
            std::vformat(fmt, std::make_format_args(args...)));
    }

    template<class... Args>
    void error(size_t tick, const char* fmt, Args&&... args)
    {
        if (logLevel == LogLevel::Off) return;

        logImpl(
            tick,
            LogSeverity::Error,
            std::vformat(fmt, std::make_format_args(args...)));
    }

    template<class T>
    void ifError(
        size_t tick,
        const char* headerMessage,
        const std::expected<T, ErrorMessage>& msg)
    {
        if (!msg) error(tick, "{}:error: {}", headerMessage, msg.error());
    }

    template<class T>
    void logOrError(
        size_t tick,
        const char* headerMessage,
        const std::expected<T, ErrorMessage>& msg)
    {
        if (msg)
            if constexpr (std::same_as<T, std::string>)
            {
                log(tick, "{}:ok: {}", headerMessage, msg.value());
            }
            else
            {
                log(tick, "{}:ok", headerMessage);
            }
        else
            error(tick, "{}:error: {}", headerMessage, msg.error());
    }

    constexpr void setLogLevel(LogLevel level) noexcept
    {
        logLevel = level;
    }

protected:
    virtual void
    logImpl(size_t tick, LogSeverity severity, const std::string& message) = 0;

protected:
    LogLevel logLevel = LogLevel::All;
};

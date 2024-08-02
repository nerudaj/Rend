#pragma once

#include <Error.hpp>
#include <format>
#include <string>

class [[nodiscard]] LoggerInterface
{
public:
    virtual ~LoggerInterface() = default;

public:
    template<class... Args>
    void log(const char* fmt, Args&&... args)
    {
        logImpl(std::vformat(fmt, std::make_format_args(args...)));
    }

    template<class T>
    void log(const std::expected<T, ErrorMessage>& msg)
    {
        if (!msg) logImpl(msg.error());
    }

    template<>
    void log(const std::expected<std::string, ErrorMessage>& msg)
    {
        if (msg)
            logImpl(msg.value());
        else
            log("error: {}", msg.error());
    }

protected:
    virtual void logImpl(const std::string& message) = 0;
};

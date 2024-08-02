#pragma once

#include <expected>
#include <stacktrace>
#include <string>

// NOTE: This header needs a complete redesign, but not as a part of this
// project

class BaseError
{
};

using ErrorMessage = std::string;

enum class ReturnFlag : bool
{
    Success = true,
    Failure = false,
};

using ExpectSuccess = std::expected<ReturnFlag, ErrorMessage>;

class [[nodiscard]] Error
{
public:
    Error(
        const std::string& message,
        const std::stacktrace& trace = std::stacktrace::current())
        : message(message), trace(trace)
    {
    }

    operator std::string() const
    {
        return std::format("[Error]\nMessage: {}\nTrace:\n{}", message, trace);
    }

private:
    std::string message;
    std::stacktrace trace;
};
#pragma once

#include "LoggerInterface.hpp"

class NullLogger final : public LoggerInterface
{
public:
    NullLogger()
    {
        setLogLevel(LogLevel::Off);
    }

public:
    void logImpl(size_t, LogSeverity, const std::string&) override {};
};
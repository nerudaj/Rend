#pragma once

#include "LoggerInterface.hpp"
#include <iostream>

class [[nodiscard]] StdoutLogger final : public LoggerInterface
{
protected:
    void logImpl(size_t, LogSeverity, const std::string& str) override
    {
        std::cout << str << std::endl;
    }
};

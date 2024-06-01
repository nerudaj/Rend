#pragma once

#include "logging/LoggerInterface.hpp"

class NullLogger final : public LoggerInterface
{
public:
    void logImpl(const std::string&) override {};
};
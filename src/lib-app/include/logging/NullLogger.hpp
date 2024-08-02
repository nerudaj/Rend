#pragma once

#include "LoggerInterface.hpp"

class NullLogger final : public LoggerInterface
{
public:
    void logImpl(const std::string&) override {};
};
#pragma once

#include "logging/LoggerInterface.hpp"
#include <Memory.hpp>
#include <filesystem>

class LoggerFactory final
{
public:
    static [[nodiscard]] mem::Rc<LoggerInterface>
    createLogger(bool enabled, const std::filesystem::path& logfile);
};

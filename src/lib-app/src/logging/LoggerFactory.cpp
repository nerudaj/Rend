#include "logging/LoggerFactory.hpp"
#include "logging/FileLogger.hpp"
#include "logging/NullLogger.hpp"

mem::Rc<LoggerInterface>
LoggerFactory::createLogger(bool enabled, const std::filesystem::path& logfile)
{
    if (enabled) return mem::Rc<FileLogger>(logfile);
    return mem::Rc<NullLogger>();
}

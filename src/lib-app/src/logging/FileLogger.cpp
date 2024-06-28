#include "logging/FileLogger.hpp"

FileLogger::FileLogger(const std::filesystem::path& logfilePath)
    : logstream(logfilePath)
{
}

void FileLogger::logImpl(const std::string& message)
{
    logstream << message << "\n";
    logstream.flush();
}

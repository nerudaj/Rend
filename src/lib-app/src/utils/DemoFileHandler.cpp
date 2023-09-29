#include <cassert>
#include <utils/DemoFileHandler.hpp>

DemoFileHandler::DemoFileHandler(
    const std::filesystem::path& path, DemoFileMode mode)
{
    stream = std::fstream(
        path,
        mode == DemoFileMode::Read
            ? std::ios_base::in
            : (std::ios_base::out | std::ios_base::trunc));
    assert(stream.good());
}

std::string DemoFileHandler::getLine()
{
    std::string line;
    std::getline(stream, line);
    return line;
}

void DemoFileHandler::writeLine(const std::string& line)
{
    stream << line << "\n";
}

void DemoFileHandler::synchronize()
{
    stream.flush();
}

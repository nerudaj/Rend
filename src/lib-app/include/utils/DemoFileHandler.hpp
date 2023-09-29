#pragma once

#include <filesystem>
#include <fstream>

enum class DemoFileMode
{
    Read,
    Write
};

class DemoFileHandler
{
public:
    [[nodiscard]] DemoFileHandler(
        const std::filesystem::path& path, DemoFileMode mode);

public:
    std::string getLine();

    void writeLine(const std::string& line);

    void synchronize();

private:
    std::fstream stream;
};

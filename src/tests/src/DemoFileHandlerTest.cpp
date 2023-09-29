#include <catch.hpp>
#include <filesystem>
#include <fstream>
#include <utils/DemoFileHandler.hpp>

constexpr const char* FILENAME = "file.txt";

void deleteDemoFile()
{
    std::filesystem::remove(FILENAME);
}

void createDemoFile()
{
    deleteDemoFile();
    auto&& save = std::ofstream(FILENAME);
    save << "line1\nline2\n";
    save.close();
    save.clear();
}

std::string getDemoFileContent()
{
    std::ifstream load(FILENAME);
    return std::string(
        (std::istreambuf_iterator<char>(load)),
        (std::istreambuf_iterator<char>()));
}

TEST_CASE("[DemoFileHandler]")
{
    SECTION("Can read file")
    {
        createDemoFile();
        auto&& handler = DemoFileHandler(FILENAME, DemoFileMode::Read);
        REQUIRE(handler.getLine() == "line1");
        REQUIRE(handler.getLine() == "line2");
    }

    SECTION("Can write file")
    {

        deleteDemoFile();
        {
            auto&& handler = DemoFileHandler(FILENAME, DemoFileMode::Write);
            handler.writeLine("wline1");
            handler.writeLine("wline2");
        }
        REQUIRE(getDemoFileContent() == "wline1\nwline2\n");
    }

    SECTION("Can overwrite existing file")
    {
        createDemoFile();
        {
            auto&& handler = DemoFileHandler(FILENAME, DemoFileMode::Write);
            handler.writeLine("wline1");
            handler.writeLine("wline2");
        }
        REQUIRE(getDemoFileContent() == "wline1\nwline2\n");
    }
}

#include "TestHelpers/TestAssets.hpp"
#include <catch.hpp>
#include <include/Utilities/ProcessCreator.hpp>
#include <iostream>

TEST_CASE("[ProcessCreator]")
{
    ProcessCreator creator;

    SECTION("Correctly executes existing batfile")
    {
        auto result =
            creator.Exec(Process::PROCESS_PATH, "param1 param2", "C:\\");
        REQUIRE(result.has_value());
    }

    SECTION("Returns error message if batfile doesn't exist")
    {
        auto result = creator.Exec("nonexistent.bat", "param1", "C:\\");

        REQUIRE_FALSE(result.has_value());
        std::cout << result.error() << std::endl;
        // There are extra whitespaces, so direct comparison is not ideal
        REQUIRE(
            result.error().find("The system cannot find the file specified.")
            != std::string::npos);
    }
}

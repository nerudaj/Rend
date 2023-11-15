#pragma once

#include <filesystem>

struct ProgramOptions
{
	std::filesystem::path binaryDir;
	std::filesystem::path rootDir;
	std::string binaryDirHash;
	std::string launchOptions;
};

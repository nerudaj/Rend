#include "include/JsonHelper.hpp"
#include <fstream>

nlohmann::json JsonHelper::loadFromFile(const std::string& path) {
	std::ifstream load(path);
	nlohmann::json result = nlohmann::json::parse(load);
	load.close();
	load.clear();

	return result;
}

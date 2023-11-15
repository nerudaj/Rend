#include "include/LogConsole.hpp"
#include "include/Configs/Sizers.hpp"
#include "DGM/classes/Utility.hpp"

Log &Log::get() {
	static Log instance;
	return instance;
}

void Log::write(const std::string& message) {
	auto &ctx = get();
	ctx.output->addLine(message);
}

void Log::write(const std::string& label, const sf::Vector2i& vec) {
	write(label + ": " + dgm::Utility::to_string(vec));
}

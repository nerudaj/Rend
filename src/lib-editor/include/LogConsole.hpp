#pragma once

#include <DGM/dgm.hpp>
#include <TGUI/TGUI.hpp>
#include <format>

class Log {
private:
	tgui::ChatBox::Ptr output;

	Log() = default;

public:
	static Log &get();

	static void write(const std::string& text);

	static void write(const std::string& label, const sf::Vector2i &vec);

	template<class... Args>
	static void write2(const char* fmt, Args&&... args) {
		const auto message = std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...));
		write(message);
	}

	void init(tgui::ChatBox::Ptr uiLogger) {
		output = uiLogger;
	}

	Log(const Log&) = delete;
	Log(Log&&) = default;
};

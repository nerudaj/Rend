#pragma once

#include <concepts>

class CommandInterface
{
public:
	CommandInterface() = default;
	CommandInterface(CommandInterface&&) = delete;
	CommandInterface(const CommandInterface&&) = delete;
	virtual ~CommandInterface() = default;

public:
	virtual void exec() = 0;
};

template<class T>
concept Command = std::derived_from<T, CommandInterface>;

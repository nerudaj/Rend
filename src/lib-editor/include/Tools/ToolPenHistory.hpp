#pragma once

#include <vector>

// Keeps track of which tiles were used in the past
// to display them on the right sidebar
class ToolPenHistory
{
private:
	std::vector<unsigned> history;

public:
	constexpr void insert(unsigned value)
	{
		auto it = std::find(begin(), end(), value);
		if (it != end()) history.erase(it);
		history.insert(begin(), value);
	}

	constexpr void prune(unsigned size)
	{
		if (history.size() > size) history.resize(size);
	}

	constexpr void clear()
	{
		history.clear();
		insert(0);
	}

	[[nodiscard]]
	constexpr std::vector<unsigned>::const_iterator begin() const noexcept
	{
		return history.begin();
	}

	[[nodiscard]]
	constexpr std::vector<unsigned>::const_iterator end() const noexcept
	{
		return history.end();
	}
};

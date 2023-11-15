#pragma once

#include <cstdint>

class PropertyTag
{
private:
	uint32_t value = 0;

	[[nodiscard]]
	PropertyTag() noexcept = default;
	PropertyTag(const PropertyTag& other) = delete;
	PropertyTag(PropertyTag&& other) = delete;
	PropertyTag& operator=(const PropertyTag& other) = delete;

public:
	~PropertyTag() = default;

public:
	[[nodiscard]]
	static PropertyTag& get() noexcept
	{
		static PropertyTag instance;
		return instance;
	}

	[[nodiscard]]
	constexpr void updateTag(uint32_t val) noexcept
	{
		if (value < val) value = val;
	}

	[[nodiscard]]
	constexpr uint32_t getNewTag() noexcept
	{
		return ++value;
	}
};

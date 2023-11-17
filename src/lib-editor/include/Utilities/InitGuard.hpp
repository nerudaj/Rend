#pragma once

#include <concepts>

/**
 *  Simple wrapper class that checks whether trivial type
 *  was initialized before use.
 */
template<class T>
//requires std::is_trivially_constructible_v<T>
class InitGuard
{
protected:
	bool initialized = false;
	T value = T();

protected:
	void checkInitialized() const noexcept
	{
		if (!initialized)
			std::terminate();
	}

public:
	InitGuard() = default;
	~InitGuard() = default;

public:
	[[nodiscard]]
	T& operator*() noexcept
	{
		checkInitialized();
		return value;
	}

	[[nodiscard]]
	const T& operator*() const noexcept
	{
		checkInitialized();
		return value;
	}

	void operator=(T newVal) noexcept
	{
		initialized = true;
		value = newVal;
	}
};

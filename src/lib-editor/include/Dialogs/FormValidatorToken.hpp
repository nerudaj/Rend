#pragma once

class FormValidatorToken
{
public:
	constexpr void invalidate() noexcept
	{
		valid = false;
	}

	constexpr void setValid() noexcept
	{
		valid = true;
	}

	[[nodiscard]]
	constexpr bool isValid() const noexcept
	{
		return valid;
	}

private:
	bool valid = true;
};

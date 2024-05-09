#pragma once

[[nodiscard]] consteval bool operator""_true(const char*, size_t) noexcept
{
	return true;
}

[[nodiscard]] consteval bool operator""_false(const char*, size_t) noexcept
{
	return false;
}

[[nodiscard]] consteval auto operator""_upercent(
	unsigned long long num) noexcept
{
	return num;
}

/*
Pixels
*/
[[nodiscard]] consteval float operator""_px(
	unsigned long long value) noexcept
{
	return static_cast<float>(value);
}

/**
  Units Per Second

  Unit of length. Voxels are defined in units and units roughly equate
  to pixels. So for 16x16 px textures, voxel is 16x16 units.
 */
[[nodiscard]] consteval float operator""_unitspersec(
	unsigned long long value) noexcept
{
	return static_cast<float>(value);
}

[[nodiscard]] consteval int operator""_damage(
	unsigned long long value) noexcept
{
	return static_cast<int>(value);
}

[[nodiscard]] consteval float operator""_seconds(
	unsigned long long value) noexcept
{
	return static_cast<float>(value);
}

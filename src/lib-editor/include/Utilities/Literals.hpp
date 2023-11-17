#pragma once

[[nodiscard]] consteval bool operator""_true(const char*, std::size_t) noexcept
{
    return true;
}

[[nodiscard]] consteval bool operator""_false(const char*, std::size_t) noexcept
{
    return false;
}

[[nodiscard]] consteval auto
operator""_upercent(unsigned long long num) noexcept
{
    return num;
}
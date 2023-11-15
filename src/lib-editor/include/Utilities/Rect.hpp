#pragma once

// bottom right coordinate is supposed to be inclusive
template<class T>
struct Rect
{
	T left = T();
	T top = T();
	T right = T();
	T bottom = T();
};

template<class T>
[[nodiscard]]
constexpr bool operator==(const Rect<T>& a, const Rect<T>& b) noexcept
{
	return a.left == b.left
		&& a.top == b.top
		&& a.right == b.right
		&& a.bottom == b.bottom;
}

using TileRect = Rect<unsigned>;
using CoordRect = Rect<int>;

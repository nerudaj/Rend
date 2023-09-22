module;

#include <type_traits>

export module Traits;

import Box;

export namespace mem
{
	template<class T>
	struct IsBox : std::false_type
	{};

	template<class T>
	struct IsBox<Box<T>> : std::true_type
	{};

	template<class T>
	struct IsRc : std::false_type
	{};

	template<class T>
	struct IsRc<Box<T>> : std::true_type
	{};
}

#pragma once

#include <type_traits>
#include "Box.hpp"
#include "Rc.hpp"

namespace mem
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
	struct IsRc<Rc<T>> : std::true_type
	{};
}

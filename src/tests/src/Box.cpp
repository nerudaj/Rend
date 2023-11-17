#include <catch.hpp>
#include "include/Utilities/GC.hpp"
#include <iostream>
#include <vector>
#include <queue>

class Base
{
public:
	virtual void foo() = 0;
	constexpr virtual ~Base() = default;
};

class Derived final : public Base
{
public:
	void foo() override
	{
		std::cout << "Derived::foo()" << std::endl;
	}
};

[[nodiscard]]
constexpr bool create_update_test()
{
	GC<int> intBox = GC<int>(10);
	*intBox += 10;
	return *intBox == 20;
}

[[nodiscard]]
constexpr bool constructs_from_derived()
{
	GC<Base> box = GC<Derived>();
	GC<Base> box2 = box;
	GC<Derived> box3 = GC<Derived>();
	GC<Base> box4 = box3;
	return true;
}

[[nodiscard]]
constexpr bool can_be_rebound()
{
	GC<int> a = GC<int>(10);
	GC<int> b = GC<int>(20);
	a = b;
	return *a == 20;
}

TEST_CASE("[GC]")
{
	SECTION("constexpr tests")
	{
		static_assert(create_update_test());
		static_assert(constructs_from_derived());
		static_assert(can_be_rebound());
	}

	SECTION("Works with std::vector")
	{
		std::vector<GC<int>> ints;
		ints.push_back(GC<int>(10));
		ints.push_back(GC<int>(20));
		ints.erase(ints.begin());
		REQUIRE(ints.size() == 1);
		REQUIRE(*ints.front() == 20);
	}

	SECTION("Works with std::queue")
	{
		std::queue<GC<int>> ints;
		ints.push(GC<int>(10));
		ints.push(GC<int>(20));
		ints.pop();
		REQUIRE(ints.size() == 1);
		REQUIRE(*ints.front() == 20);
	}
}

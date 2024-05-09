#include <Memory.hpp>
#include <catch.hpp>

class Base
{
public:
    virtual ~Base() = default;

public:
    virtual void foo() = 0;
};

class Derived : public Base
{
public:
    void foo() override {}
};

TEST_CASE("[Rc]")
{
    SECTION("Can be constructed from derived")
    {
        auto derived = mem::Rc<Derived>();
        auto base = mem::Rc<Base>(derived);
        base->foo();
    }
}
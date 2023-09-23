#include <catch.hpp>
#include <utils/RoundRobinBuffer.hpp>

namespace RoundRobinBufferTest
{

    TEST_CASE("[RoundRobinBuffer]")
    {
        SECTION("Can correctly push back")
        {
            RoundRobinBuffer<int, 6> buffer;
            buffer.pushBack(1);
            buffer.pushBack(2);
            buffer.pushBack(3);
            REQUIRE(buffer.getSize() == 3u);

            buffer.pushBack(4);
            buffer.pushBack(5);
            buffer.pushBack(6);
            REQUIRE(buffer.getSize() == 6u);
            REQUIRE(buffer[0] == 1);

            buffer.pushBack(7);
            REQUIRE(buffer.getSize() == 6u);
            REQUIRE(buffer[0] == 2);

            REQUIRE(buffer.last() == 7);
        }
    }

} // namespace RoundRobinBufferTest
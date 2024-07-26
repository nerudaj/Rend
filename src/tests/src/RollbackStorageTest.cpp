#include <catch.hpp>
#include <print>
#include <utils/RollbackManager.hpp>

struct Logger
{
    Logger()
    {
        std::println("Logger::ctor()");
    }

    Logger(Logger&&)
    {
        std::println("Logger::move()");
    }

    Logger& operator=(Logger&&)
    {
        std::println("Logger::move assignment()");
        return *this;
    }

    Logger(const Logger&)
    {
        std::println("Logger::copy()");
    }

    Logger& operator=(const Logger&)
    {
        std::println("Logger::copy assignment()");
        return *this;
    }

    ~Logger()
    {
        std::println("Logger::dtor()");
    }
};

TEST_CASE("[RollbackManager]")
{
    SECTION("There are no excessive allocations")
    {
        auto&& rs = RollbackManager<Logger, 5>();
        rs.insert(Logger(), 0);
        // The expected output is to see a single move assignment and no copies
    }

    auto&& rs = RollbackManager<int, 3>();

    SECTION("Correctly returns buffer index")
    {
        rs.insert(42, 0);
        REQUIRE(rs.getIndexForTick(0).value() == 0);
        rs.insert(43, 1);
        REQUIRE(rs.getIndexForTick(1).value() == 1);
        rs.insert(44, 2);
        REQUIRE(rs.getIndexForTick(2).value() == 2);
        rs.insert(45, 3);
        REQUIRE(rs.getIndexForTick(3).value() == 2);
        REQUIRE(!rs.getIndexForTick(0).has_value());
    }

    SECTION("Correctly unrolls, simulates and backups")
    {
        {
            int cnt = 0;
            int bakCnt = 0;
            rs.insert(42, 0);
            rs.forEachItemFromOldestToNewest(
                [&cnt](auto&, auto) { ++cnt; }, [&bakCnt](auto&) { ++bakCnt; });
            REQUIRE(cnt == 0);
            REQUIRE(bakCnt == 1);
        }

        {
            int cnt = 0;
            int bakCnt = 0;
            rs.insert(43, 1);
            rs.forEachItemFromOldestToNewest(
                [&cnt](auto&, auto) { ++cnt; }, [&bakCnt](auto&) { ++bakCnt; });
            REQUIRE(cnt == 1);
            REQUIRE(bakCnt == 1);
        }

        {
            int cnt = 0;
            int bakCnt = 0;
            rs.insert(43, 2);
            rs.forEachItemFromOldestToNewest(
                [&cnt](auto&, auto) { ++cnt; }, [&bakCnt](auto&) { ++bakCnt; });
            REQUIRE(cnt == 2);
            REQUIRE(bakCnt == 2);
        }
    }

    SECTION("forEachItemFromTick")
    {
        SECTION("If this is frame zero, only call backup")
        {
            int cnt = 0;
            int bakCnt = 0;
            rs.insert(42, 0);
            rs.forEachItemFromTick(
                0,
                [&cnt](auto&, auto) { ++cnt; },
                [&bakCnt](auto&) { ++bakCnt; });

            REQUIRE(cnt == 0);
            REQUIRE(bakCnt == 1);
        }

        SECTION("If this is a second frame, unroll once")
        {
            int cnt = 0;
            int bakCnt = 0;
            rs.insert(42, 0);
            rs.insert(43, 1);
            rs.forEachItemFromTick(
                0,
                [&cnt](auto&, auto) { ++cnt; },
                [&bakCnt](auto&) { ++bakCnt; });

            REQUIRE(cnt == 1);
            REQUIRE(bakCnt == 1);
        }

        SECTION("If this is a third frame, unroll twice")
        {
            int cnt = 0;
            int bakCnt = 0;

            rs.insert(42, 0);
            rs.insert(43, 1);
            rs.insert(44, 2);

            rs.forEachItemFromTick(
                0,
                [&cnt](auto&, auto) { ++cnt; },
                [&bakCnt](auto&) { ++bakCnt; });

            REQUIRE(cnt == 2);
            REQUIRE(bakCnt == 2);
        }

        SECTION(
            "If this is fiftieth frame frame and we want to unroll from "
            "fourthy-eight, unroll once")
        {
            int cnt = 0;
            int bakCnt = 0;

            for (auto&& tick : std::views::iota(size_t { 0 }, size_t { 50 }))
            {
                rs.insert(tick, tick);
            }

            rs.forEachItemFromTick(
                48,
                [&cnt](auto&, auto) { ++cnt; },
                [&bakCnt](auto&) { ++bakCnt; });

            REQUIRE(cnt == 1);
            REQUIRE(bakCnt == 1);
        }

        SECTION(
            "If this is fifty-first frame frame and we want to unroll from "
            "fourthy-eight, unroll twice")
        {
            int cnt = 0;
            int bakCnt = 0;

            for (auto&& tick : std::views::iota(size_t { 0 }, size_t { 51 }))
            {
                rs.insert(tick, tick);
            }

            rs.forEachItemFromTick(
                48,
                [&cnt](auto&, auto) { ++cnt; },
                [&bakCnt](auto&) { ++bakCnt; });

            REQUIRE(cnt == 2);
            REQUIRE(bakCnt == 2);
        }
    }
}
#include "RootDir.hpp"
#include <LevelD.hpp>
#include <builder/MeshBuilder.hpp>
#include <chrono>
#include <print>
#include <ranges>
#include <utils/DistanceIndex.hpp>

using msec = std::chrono::duration<double, std::milli>;

msec measure(std::function<void(void)> func)
{
    auto&& start = std::chrono::high_resolution_clock::now();
    func();
    auto&& end = std::chrono::high_resolution_clock::now();
    return end - start;
}

template<unsigned AttemptCount>
msec benchmark(std::function<void(void)> func)
{
    auto&& acc = msec {};
    for (auto&& attempt : std::views::iota(0u, AttemptCount))
    {
        std::print("Benchmark attempt {}", attempt);
        auto&& result = measure(func);
        acc += result;
        std::println(" - {}", result);
    }

    acc /= AttemptCount;

    std::println("Benchmark result {}", acc.count());
    return acc;
}

int main()
{
    LevelD lvd;
    lvd.loadFromFile(
        (ROOT_DIR / "resources" / "levels" / "neon_4P.lvd").string());

    auto&& mesh = MeshBuilder::buildMeshFromLvd(lvd);
    constexpr auto ATTEMPT_COUNT = 1;
    /*std::println("OG Index");
    std::ignore = benchmark<ATTEMPT_COUNT>(
        [&mesh] { std::ignore = DistanceIndex(mesh); });*/
    // std::println(" 23633.2024");

    // std::println("previous best 23312.20075"); // 23248.3401

    /*std::println("Fwd Scan Index");
    std::ignore = benchmark<ATTEMPT_COUNT>(
        [&mesh] { std::ignore = DistanceIndex2(mesh); });*/

    std::println("Manhattan");
    // std::ignore = benchmark<ATTEMPT_COUNT>(
    //     [&mesh] { std::ignore = DistanceIndex3(mesh); });

    std::println("Manhattan2");
    std::ignore = benchmark<ATTEMPT_COUNT>(
        [&mesh] { std::ignore = DistanceIndex4(mesh); });

    std::println("Best util 58883/209395");

    std::println("flood");
    std::ignore =
        benchmark<10>([&mesh] { std::ignore = DistanceIndex5(mesh); });
    return 0;
}
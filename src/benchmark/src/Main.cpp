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

    std::println("flood");
    std::ignore = benchmark<10>([&mesh] { std::ignore = DistanceIndex(mesh); });
    return 0;
}
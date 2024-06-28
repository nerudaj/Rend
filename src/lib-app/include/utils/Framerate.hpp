#pragma once

#include <chrono>
#include <thread>

class [[nodiscard]] Framerate final
{
public:
    explicit Framerate(const unsigned fps)
        : targetFrameTime(1'000'000 / fps), clock(), clockStart(clock.now())
    {
    }

public:
    void ensureFramerate(const std::chrono::milliseconds extraDelay = {})
    {
        auto&& clockEnd = clock.now();
        auto&& frameTime = clockEnd - clockStart;
        if (targetFrameTime > frameTime)
        {
            std::this_thread::sleep_for(
                targetFrameTime - frameTime + extraDelay);
        }
        clockStart = clock.now();
    }

private:
    std::chrono::microseconds targetFrameTime;
    std::chrono::high_resolution_clock clock;
    std::chrono::time_point<std::chrono::high_resolution_clock> clockStart;
};
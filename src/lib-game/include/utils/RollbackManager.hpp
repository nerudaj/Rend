#pragma once

#include <Error.hpp>
#include <format>
#include <functional>
#include <utils/RoundRobinBuffer.hpp>

template<class T, size_t Capacity>
class [[nodiscard]] RollbackManager final
{
public:
    using StoredType = T;
    using BufferType = RoundRobinBuffer<StoredType, Capacity>;
    using BufferIndexType = BufferType::BufferIndexType;

public:
    // insert until tick
    void insert(dgm::UniversalReference<T> auto&& item, size_t tick)
    {
        assert(buffer.isEmpty() || tick > endIterator.tick);
        buffer.pushBack(std::forward<decltype(item)>(item));
        endIterator = { .tick = tick, .bufferIdx = buffer.getSize() - 1 };
    }

    [[nodiscard]] constexpr auto&& get(this auto&& self, size_t tick) noexcept
    {
        return self.buffer[self.getIndexForTick(tick).value()];
    }

    [[nodiscard]] constexpr std::expected<BufferIndexType, ErrorMessage>
    getIndexForTick(size_t tick) noexcept
    {
        if (isTickTooNew(tick) || isTickTooOld(tick))
            return std::unexpected(std::format(
                "Requested index is not in buffer, endIterator.tick: {}, tick: "
                "{}, Capacity: {}",
                endIterator.tick,
                tick,
                Capacity));

        return endIterator.bufferIdx - (endIterator.tick - tick);
    }

    [[nodiscard]] constexpr size_t getLastInsertedTick() const noexcept
    {
        return endIterator.tick;
    }

    template<
        size_t HowMuchToUnroll = Capacity,
        class = std::enable_if<HowMuchToUnroll <= Capacity>>
    constexpr void forEachItemFromOldestToNewest(
        std::function<void(T&, bool)> simulateCallback,
        std::function<void(T&)> backupToCallback)
    {
        static_assert(HowMuchToUnroll <= Capacity);
        assert(buffer.getSize() >= 1u);

        const auto&& startIdx = buffer.getSize() < HowMuchToUnroll
                                    ? BufferIndexType(0)
                                    : buffer.getSize() - HowMuchToUnroll;
        // Max index is always the next frame
        const auto&& endIdx = buffer.getSize() - BufferIndexType(1);

        for (auto idx = startIdx; idx < endIdx; ++idx)
        {
            // Write the simulated state into the next frame
            simulateCallback(buffer[idx], idx + 1 == endIdx);
            backupToCallback(buffer[idx + 1]);
        }

        if (isThisTheEndOfFrameZero())
        {
            backupToCallback(buffer.last());
        }
    }

    [[nodiscard]] constexpr bool isThisTheEndOfFrameZero() const noexcept
    {
        return buffer.getSize() == 1u;
    }

    [[nodiscard]] constexpr bool isTickTooOld(size_t tick) const noexcept
    {
        return endIterator.tick > tick && (endIterator.tick - tick) >= Capacity;
    }

    [[nodiscard]] constexpr bool isTickHalfwayTooOld(size_t tick) const noexcept
    {
        return endIterator.tick > tick
               && (endIterator.tick - tick) >= Capacity / 2u;
    }

    [[nodiscard]] constexpr bool isTickTooNew(size_t tick) const noexcept
    {
        return buffer.isEmpty() || tick > endIterator.tick;
    }

private:
    [[nodiscard]] constexpr auto&& last(this auto&& self) noexcept
    {
        return self.buffer.last();
    }

private:
    struct EndIterator
    {
        size_t tick = 0;
        size_t bufferIdx = 0;
    } endIterator;

    BufferType buffer;
};

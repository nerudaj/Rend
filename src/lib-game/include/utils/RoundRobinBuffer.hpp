#include <DGM/dgm.hpp>
#include <array>

template<class T, size_t C>
class RoundRobinBuffer
{
public:
    using DataType = T;
    using BufferIndexType = decltype(C);

public:
    void pushBack(dgm::UniversalReference<T> auto&& item)
    {
        data[endIndex] = std::forward<decltype(item)>(item);
        used = std::clamp(used + 1u, size_t(0), C);
        endIndex = (endIndex + 1u) % C;
    }

    constexpr auto&&
    operator[](this auto&& self, BufferIndexType index) noexcept
    {
        if (self.getSize() == C)
            return self.data[(self.endIndex + index) % C];
        else
            return self.data[index];
    }

    auto& last(this auto&& self) noexcept
    {
        assert(self.getSize() != 0);
        return self[self.getSize() - 1];
    }

    [[nodiscard]] constexpr BufferIndexType getSize() const noexcept
    {
        return used;
    }

    [[nodiscard]] constexpr BufferIndexType getCapacity() const noexcept
    {
        return C;
    }

    [[nodiscard]] constexpr bool isEmpty() const noexcept
    {
        return getSize() == 0u;
    }

private:
    BufferIndexType endIndex = 0;
    BufferIndexType used = 0;
    std::array<T, C> data;
};
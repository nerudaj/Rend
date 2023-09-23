#include <DGM/dgm.hpp>

template<class T, unsigned C>
class RoundRobinBuffer
{
public:
    using DataType = T;

public:
    void pushBack(dgm::UniversalReference<T> auto&& item)
    {
        data[endIndex] = std::forward<decltype(item)>(item);
        used = std::clamp(used + 1u, 0u, C);
        endIndex = (endIndex + 1u) % C;
    }

    auto& operator[](this auto&& self, unsigned index)
    {
        if (self.getSize() == C)
            return self.data[(self.endIndex + index) % C];
        else
            return self.data[index];
    }

    auto& last(this auto&& self)
    {
        return self[self.getSize() - 1];
    }

    [[nodiscard]] constexpr unsigned getSize() const noexcept
    {
        return used;
    }

    [[nodiscard]] constexpr unsigned getCapacity() const noexcept
    {
        return C;
    }

private:
    unsigned endIndex = 0;
    unsigned used = 0;
    std::array<T, C> data;
};
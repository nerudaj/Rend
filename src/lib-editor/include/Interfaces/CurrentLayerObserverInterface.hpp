#pragma once

#include <limits>

class LayerObserverInterface
{
public:
    virtual ~LayerObserverInterface() = default;

public:
    [[nodiscard]] virtual std::size_t getCurrentLayerId() const noexcept = 0;

    [[nodiscard]] virtual std::size_t getLayerCount() const noexcept = 0;
};

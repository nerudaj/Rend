#pragma once

#include "Interfaces/CurrentLayerObserverInterface.hpp"
#include <string>

class LayerController final : public LayerObserverInterface
{
public: // LayerObserverInterface
    std::size_t getCurrentLayerId() const noexcept override;

    std::size_t getLayerCount() const noexcept override;

public:
    void moveUp();

    void moveDown();

    std::string toString() const;

private:
    std::size_t layerId = 0;
};
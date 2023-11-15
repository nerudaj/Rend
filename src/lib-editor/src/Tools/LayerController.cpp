#include "Tools/LayerController.hpp"
#include <string>

std::size_t LayerController::getCurrentLayerId() const noexcept
{
    return layerId;
}

std::size_t LayerController::getLayerCount() const noexcept
{
    return 3;
}

void LayerController::moveUp()
{
    if (layerId + 1 < getLayerCount())
    {
        ++layerId;
    }
}

void LayerController::moveDown()
{
    if (layerId > 0)
    {
        --layerId;
    }
}

std::string LayerController::toString() const
{
    switch (getCurrentLayerId())
    {
    case 0:
        return "Bottom layer";
    case 1:
        return "Middle layer";
    case 2:
        return "Upper layer";
    default:
        return "ERROR: Too many layers";
    }
}

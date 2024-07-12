#pragma once

#include "Interfaces/ToolPropertyWithImageInterface.hpp"
#include <TGUI/TGUI.hpp>

class MeshToolProperty final : public ToolPropertyWithImageInterface
{
public:
    MeshToolProperty(
        tgui::Texture previewImage,
        uint32_t tileX,
        uint32_t tileY,
        uint32_t layerIdx,
        uint16_t tileValue,
        bool blocking,
        bool defaultBlocking)
        : ToolPropertyWithImageInterface(previewImage)
        , tileX(tileX)
        , tileY(tileY)
        , layerIdx(layerIdx)
        , tileValue(tileValue)
        , blocking(blocking)
        , defaultBlocking(defaultBlocking)
    {
    }

public:
    void fillEditDialogInternal(tgui::ScrollablePanel::Ptr& panel) override;

public:
    uint32_t tileX = 0;
    uint32_t tileY = 0;
    uint32_t layerIdx = 0;
    uint16_t tileValue = 0;
    bool blocking = false;
    bool defaultBlocking = false;
};

#pragma once

#include "Interfaces/ToolPropertyWithImageInterface.hpp"
#include <LevelD.hpp>
#include <TGUI/TGUI.hpp>

class ItemToolProperty : public ToolPropertyWithImageInterface
{
public:
    ItemToolProperty(
        tgui::Texture previewImage, std::size_t itemId, LevelD::Thing data)
        : ToolPropertyWithImageInterface(previewImage)
        , itemId(itemId)
        , data(data)
    {
    }

public:
    void fillEditDialogInternal(tgui::ScrollablePanel::Ptr& panel) override;

public:
    std::size_t itemId;
    LevelD::Thing data;
};

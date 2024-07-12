#pragma once

#include "Interfaces/ToolPropertyInterface.hpp"
#include <TGUI/TGUI.hpp>

class ToolPropertyWithImageInterface : public ToolPropertyInterface
{
public:
    ToolPropertyWithImageInterface(tgui::Texture previewImage)
        : previewImage(previewImage)
    {
    }

public:
    void fillEditDialog(tgui::Panel::Ptr& panel) final override;

protected:
    virtual void fillEditDialogInternal(tgui::ScrollablePanel::Ptr& panel) = 0;

protected:
    tgui::Texture previewImage;
};
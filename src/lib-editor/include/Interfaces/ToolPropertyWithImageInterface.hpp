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
    void fillEditDialog(
        tgui::Panel::Ptr& panel,
        FormValidatorToken& formValidatorToken) final override;

protected:
    virtual void fillEditDialogInternal(
        tgui::Panel::Ptr& panel, FormValidatorToken& formValidatorToken) = 0;

protected:
    tgui::Texture previewImage;
};
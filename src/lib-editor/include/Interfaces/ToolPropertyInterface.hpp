#pragma once

#include "Dialogs/FormValidatorToken.hpp"
#include "Utilities/PropertyTag.hpp"
#include <DGM/dgm.hpp>
#include <TGUI/TGUI.hpp>

class ToolPropertyInterface
{
public:
    virtual ~ToolPropertyInterface() = default;

public:
    virtual void fillEditDialog(
        tgui::Panel::Ptr& panel, FormValidatorToken& formValidatorToken) = 0;
};

class NullToolProperty2 final : public ToolPropertyInterface
{
public:
    void fillEditDialog(tgui::Panel::Ptr&, FormValidatorToken&) override {}
};

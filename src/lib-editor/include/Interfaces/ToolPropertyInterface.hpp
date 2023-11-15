#pragma once

#include "include/Utilities/PropertyTag.hpp"

#include <DGM/dgm.hpp>
#include <TGUI/TGUI.hpp>

#include "include/Dialogs/FormValidatorToken.hpp"

class ToolPropertyInterface
{
public:
	~ToolPropertyInterface() = default;

public:
	virtual void fillEditDialog(
		tgui::Panel::Ptr& panel,
		FormValidatorToken& formValidatorToken) = 0;
};

class NullToolProperty2 final : public ToolPropertyInterface
{
public:
	void fillEditDialog(
		tgui::Panel::Ptr&,
		FormValidatorToken&) override
	{}
};

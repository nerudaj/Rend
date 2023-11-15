#pragma once

#include "include/Interfaces/ToolPropertyWithImageInterface.hpp"

#include <TGUI/TGUI.hpp>
#include <LevelD.hpp>

class ItemToolProperty : public ToolPropertyWithImageInterface
{
public:
	ItemToolProperty(
		tgui::Texture previewImage,
		std::size_t itemId,
		LevelD::Thing data)
		: ToolPropertyWithImageInterface(previewImage)
		, itemId(itemId)
		, data(data)
	{}

public:
	void fillEditDialogInternal(
		tgui::Panel::Ptr& panel,
		FormValidatorToken& formValidatorToken) override;

public:
	std::size_t itemId;
	LevelD::Thing data;
};

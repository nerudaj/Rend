#include "include/ToolProperties/ItemToolProperty.hpp"
#include "include/Dialogs/DialogBuilderHelper.hpp"

void ItemToolProperty::fillEditDialogInternal(
	tgui::Panel::Ptr& panel,
	FormValidatorToken& formValidatorToken)
{
	using namespace DialogBuilderHelper;

	auto dst = tgui::ScrollablePanel::create();
	panel->add(dst);

	constexpr bool DISABLED = false;

	addOption(dst, formValidatorToken, "Item ID:", "Unique identifier of the object", data.id, 0, DISABLED);
	addOption(dst, formValidatorToken, "X coordinate:", "Measured in pixels from top-left corner", data.x, 1);
	addOption(dst, formValidatorToken, "Y coordinate:", "Measured in pixels from top-left corner", data.y, 2);
	addOption(dst, formValidatorToken, "Layer ID:", "ID of layer where trigger is located", data.layerId, 3, DISABLED);
	addOption(dst, formValidatorToken, "Tag:", "Value used to group related objects", data.tag, 4, true, true);
	addOption(dst, formValidatorToken, "Flags:", "16 bit value to alter behaviour of this object", data.flags, 5);
	addOption(dst, "Metadata:", "Text field for custom data", data.metadata, 6);
}

#include "include/ToolProperties/MeshToolProperty.hpp"
#include "include/Dialogs/DialogBuilderHelper.hpp"

void MeshToolProperty::fillEditDialogInternal(
		tgui::Panel::Ptr& panel,
		FormValidatorToken& formValidatorToken)
{
	using namespace DialogBuilderHelper;

	auto dst = tgui::ScrollablePanel::create();
	panel->add(dst);

	constexpr bool DISABLED = false;

	addOption(dst, formValidatorToken, "Tile X:", "X coordinate of the tile", tileX, 0, DISABLED);
	addOption(dst, formValidatorToken, "Tile Y:", "Y coordinate of the tile", tileY, 1, DISABLED);
	addOption(dst, formValidatorToken, "Layer ID:", "In which layer is tile located", layerId, 2, DISABLED);
	addOption(dst, "Impassable:", "Whether this tile blocks the player", blocking, 3);
	addOption(dst, "Impassable by default:", "Whether this type of tile is impassable by default", defaultBlocking, 4, DISABLED);
}

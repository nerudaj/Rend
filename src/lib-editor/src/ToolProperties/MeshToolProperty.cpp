#include "ToolProperties/MeshToolProperty.hpp"
#include "Dialogs/DialogBuilderHelper.hpp"
#include <Configs/Strings.hpp>
#include <FormBuilder.hpp>
#include <WidgetBuilder.hpp>

void MeshToolProperty::fillEditDialogInternal(
    tgui::Panel::Ptr& panel, FormValidatorToken& formValidatorToken)
{
    panel->add(
        FormBuilder()
            .addOption(
                Strings::Dialog::EditProperty::TILE_X,
                WidgetBuilder::createNumericInput<uint32_t>(tileX, [](auto) {}),
                { .disabled = true,
                  .tooltipText =
                      Strings::Dialog::EditProperty::TILE_X_TOOLTIP })
            .addOption(
                Strings::Dialog::EditProperty::TILE_Y,
                WidgetBuilder::createNumericInput<uint32_t>(tileY, [](auto) {}),
                { .disabled = true,
                  .tooltipText =
                      Strings::Dialog::EditProperty::TILE_Y_TOOLTIP })
            .addOption(
                Strings::Dialog::EditProperty::LAYER_ID,
                WidgetBuilder::createNumericInput<uint32_t>(
                    layerIdx, [](auto) {}),
                { .disabled = true,
                  .tooltipText =
                      Strings::Dialog::EditProperty::LAYER_ID_TOOLTIP })
            .addOption(
                Strings::Dialog::EditProperty::IS_IMPASSABLE,
                WidgetBuilder::createCheckbox(
                    blocking, [&](bool newVal) { blocking = newVal; }),
                { .tooltipText =
                      Strings::Dialog::EditProperty::IS_IMPASSABLE_TOOLTIP })
            .addOption(
                Strings::Dialog::EditProperty::IS_DEFAULT_IMPASSABLE,
                WidgetBuilder::createCheckbox(defaultBlocking, [](auto) {}),
                { .disabled = true,
                  .tooltipText = Strings::Dialog::EditProperty::
                      IS_DEFAULT_IMPASSABLE_TOOLTIP })
            .build());
    /*
    using namespace DialogBuilderHelper;

    auto dst = tgui::ScrollablePanel::create();
    panel->add(dst);

    constexpr bool DISABLED = false;

    addOption(
        dst,
        formValidatorToken,
        "Tile X:",
        "X coordinate of the tile",
        tileX,
        0,
        DISABLED);
    addOption(
        dst,
        formValidatorToken,
        "Tile Y:",
        "Y coordinate of the tile",
        tileY,
        1,
        DISABLED);
    addOption(
        dst,
        formValidatorToken,
        "Layer ID:",
        "In which layer is tile located",
        layerIdx,
        2,
        DISABLED);
    addOption(
        dst, "Impassable:", "Whether this tile blocks the player", blocking, 3);
    addOption(
        dst,
        "Impassable by default:",
        "Whether this type of tile is impassable by default",
        defaultBlocking,
        4,
        DISABLED);*/
}

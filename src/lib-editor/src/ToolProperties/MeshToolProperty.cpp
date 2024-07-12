#include "ToolProperties/MeshToolProperty.hpp"
#include "Dialogs/DialogBuilderHelper.hpp"
#include <Configs/Strings.hpp>
#include <FormBuilder.hpp>
#include <WidgetBuilder.hpp>

void MeshToolProperty::fillEditDialogInternal(tgui::ScrollablePanel::Ptr& panel)
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
}

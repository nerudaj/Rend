#include "ToolProperties/ItemToolProperty.hpp"
#include "Dialogs/DialogBuilderHelper.hpp"
#include <Configs/Strings.hpp>
#include <FormBuilder.hpp>
#include <WidgetBuilder.hpp>

void ItemToolProperty::fillEditDialogInternal(tgui::ScrollablePanel::Ptr& panel)
{
    panel->add(
        FormBuilder()
            .addOption(
                Strings::Dialog::EditProperty::ITEM_ID,
                WidgetBuilder::createNumericInput<uint32_t>(
                    data.id, [](auto) {}),
                { .disabled = true,
                  .tooltipText =
                      Strings::Dialog::EditProperty::ITEM_ID_TOOLTIP })
            .addOption(
                Strings::Dialog::EditProperty::ITEM_X,
                WidgetBuilder::createNumericInput<uint32_t>(
                    data.x, [&](auto x) { data.x = x; }),
                { .tooltipText =
                      Strings::Dialog::EditProperty::ITEM_POS_TOOLTIP })
            .addOption(
                Strings::Dialog::EditProperty::ITEM_Y,
                WidgetBuilder::createNumericInput<uint32_t>(
                    data.y, [&](auto y) { data.y = y; }),
                { .tooltipText =
                      Strings::Dialog::EditProperty::ITEM_POS_TOOLTIP })
            .addOption(
                Strings::Dialog::EditProperty::ITEM_FLAGS,
                WidgetBuilder::createNumericInput<uint16_t>(
                    data.flags, [&](auto f) { data.flags = f; }),
                { .tooltipText =
                      Strings::Dialog::EditProperty::ITEM_FLAGS_TOOLTIP })
            .build());
}

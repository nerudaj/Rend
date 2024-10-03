#include "ToolProperties/ItemToolProperty.hpp"
#include <Configs/Strings.hpp>
#include <FormBuilder.hpp>
#include <WidgetBuilder.hpp>
#include <enums/SpawnRotation.hpp>

static constexpr [[nodiscard]] std::string
spawnRotationToString(SpawnRotation rot)
{
    switch (rot)
    {
        using enum SpawnRotation;
    case Right:
        return "Right";
    case DownRight:
        return "DownRight";
    case Down:
        return "Down";
    case DownLeft:
        return "DownLeft";
    case Left:
        return "Left";
    case UpLeft:
        return "UpLeft";
    case Up:
        return "Up";
    case UpRight:
        return "UpRight";
    default:
        throw std::runtime_error("Invalid value for SpawnRotation!");
    }
}

void ItemToolProperty::fillEditDialogInternal(tgui::ScrollablePanel::Ptr& panel)
{
    static_assert(std::same_as<
                  decltype(data.flags),
                  std::underlying_type_t<SpawnRotation>>);

    auto items =
        std::views::iota(
            std::to_underlying(SpawnRotation::Right),
            std::to_underlying(SpawnRotation::UpRight) + 1u)
        | std::views::transform(
            [](auto rot)
            { return spawnRotationToString(static_cast<SpawnRotation>(rot)); })
        | std::ranges::to<std::vector>();

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
                Strings::Dialog::EditProperty::ITEM_ROTATION,
                WidgetBuilder::createDropdown(
                    items,
                    spawnRotationToString(
                        static_cast<SpawnRotation>(data.flags)),
                    [&](size_t idx)
                    { data.flags = static_cast<decltype(data.flags)>(idx); }),
                { .tooltipText =
                      Strings::Dialog::EditProperty::ITEM_ROTATION_TOOLTIP })
            .build());
}

#include "FormBuilder.hpp"
#include "WidgetBuilder.hpp"
#include <Configs/Strings.hpp>
#include <Dialogs/MapPickerDialog.hpp>
#include <ranges>

MapPickerDialog::MapPickerDialog(
    mem::Rc<Gui> gui, const std::vector<MapSettingsForPicker>& settings)
    : DialogInterface(
          gui, "MapPickerDialogId", Strings::Dialog::Title::SELECT_MAPS)
    , maps(settings)
{
}

void MapPickerDialog::buildLayoutImpl(tgui::Panel::Ptr panel)
{
    auto&& builder = FormBuilder();

    for (auto&& [idx, map] : std::views::enumerate(maps))
    {
        std::ignore = builder.addOption(
            map.name,
            WidgetBuilder::createCheckbox(
                map.enabled,
                [this, idx](bool val) { maps[idx].enabled = val; }));
    }

    panel->add(builder.build());
}

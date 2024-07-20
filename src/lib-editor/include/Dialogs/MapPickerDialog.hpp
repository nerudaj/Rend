#pragma once

#include <Dialogs/DialogInterface.hpp>
#include <vector>

struct MapSettingsForPicker
{
    std::string name;
    bool enabled;
};

class MapPickerDialog : public DialogInterface
{
public:
    MapPickerDialog(
        mem::Rc<Gui> gui, const std::vector<MapSettingsForPicker>& settings);

public:
    [[nodiscard]] constexpr const std::vector<MapSettingsForPicker>&
    getMapSettings() const noexcept
    {
        return maps;
    }

private:
    void buildLayoutImpl(tgui::Panel::Ptr panel) override;

private:
    std::vector<MapSettingsForPicker> maps;
};

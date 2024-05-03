#pragma once

#include <Dialogs/DialogBase.hpp>
#include <vector>

struct MapSettingsForPicker
{
    std::string name;
    bool enabled;
};

class MapPickerDialog : public ModernDialogInterface
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

#include "Dialogs/ResizeLevelDialog.hpp"

constexpr const char* INPUT_LEVEL_WIDTH_ID = "InputLevelWidth";
constexpr const char* INPUT_LEVEL_HEIGHT_ID = "InputLevelHeight";
constexpr const char* INPUT_NO_TRANSLATE_ID = "InputNoTranslation";

unsigned ResizeDialog::getLevelWidth() const
{
    return std::stoul(getEditboxValue(INPUT_LEVEL_WIDTH_ID));
}

unsigned ResizeDialog::getLevelHeight() const
{
    return std::stoul(getEditboxValue(INPUT_LEVEL_HEIGHT_ID));
}

bool ResizeDialog::isTranslationDisabled() const
{
    return gui->get<tgui::CheckBox>(INPUT_NO_TRANSLATE_ID)->isChecked();
}

ResizeDialog::ResizeDialog(mem::Rc<Gui> gui)
    : DialogInterface(
        gui,
        "ResizeLevelDialog",
        "Resize Level",
        std::vector<OptionLine> {
            OptionInput { "Level width:", INPUT_LEVEL_WIDTH_ID, "20" },
            OptionInput { "Level height:", INPUT_LEVEL_HEIGHT_ID, "10" },
            OptionCheckbox {
                "No translations:", INPUT_NO_TRANSLATE_ID, false } })
{
}

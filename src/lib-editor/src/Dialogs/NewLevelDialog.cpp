#include "Dialogs/NewLevelDialog.hpp"
#include "Globals.hpp"

constexpr const char* INPUT_LEVEL_WIDTH_ID = "InputLevelWidth";
constexpr const char* INPUT_LEVEL_HEIGHT_ID = "InputLevelHeight";
constexpr const char* INPUT_LEVEL_CONFIG_ID = "InputLevelConfig";

void NewLevelDialog::customOpenCode()
{
    if (!configPath.has_value()) return;

    auto box = gui->get<tgui::EditBox>(INPUT_LEVEL_CONFIG_ID);
    box->setText(configPath.value());
}

unsigned NewLevelDialog::getLevelWidth() const
{
    return std::stoul(getEditboxValue(INPUT_LEVEL_WIDTH_ID));
}

unsigned NewLevelDialog::getLevelHeight() const
{
    return std::stoul(getEditboxValue(INPUT_LEVEL_HEIGHT_ID));
}

std::string NewLevelDialog::getConfigPath() const
{
    return getEditboxValue(INPUT_LEVEL_CONFIG_ID);
}

NewLevelDialog::NewLevelDialog(
    mem::Rc<Gui> gui,
    mem::Rc<FileApiInterface> fileApi,
    const std::optional<std::string>& configPath)
    : DialogInterface(
        gui,
        "ModalNewLevel",
        "New level",
        std::vector<OptionLine> {
            { OptionInput { "Level width:", INPUT_LEVEL_WIDTH_ID, "20" },
              OptionInput { "Level height:", INPUT_LEVEL_HEIGHT_ID, "10" },
              OptionInputWithButton {
                  "Config file:",
                  INPUT_LEVEL_CONFIG_ID,
                  "Pick a config file",
                  [&, gui, fileApi]
                  {
                      auto box = gui->get<tgui::EditBox>(INPUT_LEVEL_CONFIG_ID);
                      auto fileName = fileApi->getOpenFileName(JSON_FILTER);
                      box->setText(fileName.value_or(""));
                  } } } })
    , configPath(configPath)
{
}

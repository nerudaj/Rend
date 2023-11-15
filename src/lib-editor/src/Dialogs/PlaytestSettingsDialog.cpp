#include "Dialogs/PlaytestSettingsDialog.hpp"

constexpr const char* INPUT_BINARY_PATH = "InputBinaryPath";
constexpr const char* INPUT_WORKING_DIR = "InputWorkingDir";
constexpr const char* INPUT_LAUNCH_OPTIONS = "InputLaunchOptions";

PlaytestSettingsDialog::PlaytestSettingsDialog(
    mem::Rc<PlaytestLauncherOptions> options,
    mem::Rc<Gui> gui,
    mem::Rc<FileApiInterface> fileApi)
    : PlaytestSettingsDialogInterface(
        gui,
        "PlaytestSettingsDialog",
        "Playtest Settings",
        std::vector<OptionLine> {
            OptionDeferredInputWithButton {
                "Binary to launch:",
                INPUT_BINARY_PATH,
                [options] { return options->pathToBinary.string(); },
                [&, gui, fileApi]
                {
                    auto box = gui->gui.get<tgui::EditBox>(INPUT_BINARY_PATH);
                    auto fileName = fileApi->getOpenFileName(EXES_FILTER);
                    box->setText(fileName.value_or(""));
                } },
            // clang-format off
            OptionDeferredInputWithButton {
                "Working directory:",
                INPUT_WORKING_DIR,
                [options] { return options->workingDirectory.string(); },
                [&, gui, fileApi]
                {
                    auto box = gui->get<tgui::EditBox>(INPUT_WORKING_DIR);
                    auto folderPath = fileApi->selectFolder();
                    if (folderPath.has_value())
                        box->setText(folderPath->string());
                } },
            // clang-format on
            OptionText { "Use variable $(LevelPath) to pass currently "
                         "edited level in command-line parameters",
                         2 },
            OptionDeferredInput { "Cmd parameters:",
                                  INPUT_LAUNCH_OPTIONS,
                                  [options] { return options->parameters; } } })
{
}

std::filesystem::path PlaytestSettingsDialog::getBinaryPath() const
{
    return getEditboxValue(INPUT_BINARY_PATH);
}

std::string PlaytestSettingsDialog::getLaunchParameters() const
{
    return getEditboxValue(INPUT_LAUNCH_OPTIONS);
}

std::filesystem::path PlaytestSettingsDialog::getWorkingDirPath() const
{
    return getEditboxValue(INPUT_WORKING_DIR);
}

#include "Launcher/PlaytestLauncher.hpp"
#include "Configs/Strings.hpp"
#include "LogConsole.hpp"
#include <functional>
#include <optional>

PlaytestLauncher::PlaytestLauncher(
    mem::Rc<PlaytestLauncherOptions> options,
    mem::Rc<ShortcutEngineInterface> shortcutEngine,
    mem::Rc<ProcessCreatorInterface> processCreator,
    mem::Rc<PlaytestSettingsDialogInterface> dialogPlaytestSettings,
    std::function<std::string()> getCurrentLevelPathCallback) noexcept
    : options(options)
    , shortcutEngine(shortcutEngine)
    , processCreator(processCreator)
    , dialogPlaytestSettings(dialogPlaytestSettings)
    , getCurrentLevelPathCallback(std::move(getCurrentLevelPathCallback))
{
}

void PlaytestLauncher::buildContextMenu(tgui::MenuBar::Ptr menu)
{
    menu->addMenu(Strings::Launcher::CTX_MENU_NAME);

    auto addMenuItem = [this, &menu](
                           auto&& label,
                           std::function<void(void)> callback,
                           std::optional<sf::Keyboard::Key> shortcut = {})
    {
        menu->addMenuItem(std::forward<decltype(label)>(label));
        menu->connectMenuItem(
            Strings::Launcher::CTX_MENU_NAME, label, callback);

        if (shortcut.has_value())
        {
            shortcutEngine->registerShortcut(
                "PlaytestLauncherShortcuts",
                { false, false, shortcut.value() },
                callback);
        }
    };

    using namespace Strings::Launcher::ContextMenu;

    addMenuItem(
        PLAYTEST, [this] { handlePlaytestExecuted(); }, sf::Keyboard::F5);
    addMenuItem(CONFIGURE_LAUNCH, [this] { handleConfigureLaunchOptions(); });
}

template<class T>
[[nodiscard]] std::string
replaceString(std::string str, T&& from, const std::string& to)
{
    auto match = str.find(from);
    if (match == std::string::npos) return std::move(str);

    str.replace(match, std::char_traits<char>::length(from), to);
    return std::move(str);
}

void PlaytestLauncher::handlePlaytestExecuted()
{
    // TODO: just jump to another state
    auto result = processCreator->Exec(
        options->pathToBinary,
        replaceString(
            options->parameters, "$(LevelPath)", getCurrentLevelPathCallback()),
        options->workingDirectory);
    if (!result.has_value())
    {
        Log::write2("Launching playtest failed with error: {}", result.error());
    }
}

void PlaytestLauncher::handleConfigureLaunchOptions()
{
    dialogPlaytestSettings->open(
        [&]
        {
            options->pathToBinary = dialogPlaytestSettings->getBinaryPath();
            options->parameters = dialogPlaytestSettings->getLaunchParameters();
            options->workingDirectory =
                dialogPlaytestSettings->getWorkingDirPath();
        });
}

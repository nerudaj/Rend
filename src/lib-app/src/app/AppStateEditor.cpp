#include "app/AppStateEditor.hpp"
#include "Configs/Sizers.hpp"
#include "Configs/Strings.hpp"
#include "Dialogs/NewLevelDialog.hpp"
#include "Dialogs/PlaytestSettingsDialog.hpp"
#include "Editor/Editor.hpp"
#include "Editor/NullEditor.hpp"
#include "Launcher/NullPlaytestLauncher.hpp"
#include "Launcher/PlaytestLauncher.hpp"
#include "Utilities/ProcessCreator.hpp"
#include <cmath>

void AppStateEditor::handleExit(YesNoCancelDialogInterface& confirmExitDialog)
{
    if (unsavedChanges)
    {
        using namespace Strings::Dialog;

        confirmExitDialog.open(
            Title::WARNING,
            Message::UNSAVED_CHANGES,
            [&](UserChoice choice)
            {
                if (choice == UserChoice::Cancelled)
                    return;
                else if (choice == UserChoice::Confirmed)
                    handleSaveLevel();
                else
                    unsavedChanges = false;
                handleExit(confirmExitDialog);
            });
    }
    else
    {
        app.popState();
    }
}

void AppStateEditor::setupFont()
{
    gui->gui.setFont(resmgr->get<tgui::Font>("cruft.ttf").value().get());
}

std::optional<std::string> AppStateEditor::getNewSavePath()
{
    auto result = fileApi->getSaveFileName(LVLD_FILTER);
    return result.transform(
        [](const std::string& s) -> std::string
        { return s.ends_with(".lvd") ? s : s + ".lvd"; });
}

void AppStateEditor::input()
{
    const sf::Vector2i mousePos =
        sf::Mouse::getPosition(app.window.getWindowContext());

    sf::Event event;
    while (app.window.pollEvent(event))
    {
        if (clickPreventer.shouldPreventEvents()) continue;

        if (event.type == sf::Event::Closed)
        {
            handleExit(*dialogConfirmExit);
        }
        else if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Enter)
            {
                if (dialogNewLevel.isOpen())
                {
                    newLevelDialogCallback();
                    dialogNewLevel.close();
                }
            }
        }
        else if (event.type == sf::Event::GainedFocus)
        {
            sf::Event releaseKeys;
            releaseKeys.type = sf::Event::KeyReleased;
            releaseKeys.key.code = sf::Keyboard::LShift;
            shortcutEngine->handleEvent(releaseKeys);
            releaseKeys.key.code = sf::Keyboard::LControl;
            shortcutEngine->handleEvent(releaseKeys);

            clickPreventer.preventClickForNextNFrames(30);
        }

        gui->gui.handleEvent(event);
        editor->handleEvent(event, mousePos);
        shortcutEngine->handleEvent(event);
    }
}

void AppStateEditor::update()
{
    clickPreventer.update();

    if (commandQueue->isEmpty()) return;

    unsavedChanges = true;
    updateWindowTitle();

    commandQueue->processAll();
}

void AppStateEditor::draw()
{
    // Drawing canvas
    canvas->clear();

    editor->draw();

    canvas->display();

    // Whole window
    gui->gui.draw();
}

AppStateEditor::AppStateEditor(
    dgm::App& app,
    mem::Rc<Gui> gui,
    mem::Rc<const dgm::ResourceManager> resmgr,
    mem::Rc<Settings> settings,
    mem::Rc<FileApiInterface> fileApi,
    mem::Rc<ShortcutEngineInterface> shortcutEngine,
    mem::Rc<YesNoCancelDialogInterface> dialogConfirmExit,
    mem::Rc<ErrorInfoDialogInterface> dialogErrorInfo)
    : dgm::AppState(app)
    , gui(gui)
    , resmgr(resmgr)
    , settings(settings)
    , fileApi(fileApi)
    , shortcutEngine(shortcutEngine)
    , dialogConfirmExit(dialogConfirmExit)
    , dialogErrorInfo(dialogErrorInfo)
    , editor(mem::Box<NullEditor>())
    , dialogNewLevel(gui, fileApi, configPath)
    , dialogUpdateConfigPath(gui, fileApi)
    , playtestLauncher(mem::Box<NullPlaytestLauncher>())
{
    configPath =
        (settings->cmdSettings.resourcesDir / "editor-config.json").string();

    auto&& launcherOptions =
        mem::Rc<PlaytestLauncherOptions>({ "playtestBinaryPath",
                                           "playtestLaunchOptions",
                                           "playtestWorkDirPath" });
    playtestLauncher = mem::Box<PlaytestLauncher>(
        launcherOptions,
        shortcutEngine,
        mem::Rc<ProcessCreator>(),
        mem::Rc<PlaytestSettingsDialog>(launcherOptions, gui, fileApi),
        [&] { return savePath; });

    try
    {
        gui->theme.load(
            (settings->cmdSettings.resourcesDir / "editor/TransparentGrey.txt")
                .string());
        setupFont();
    }
    catch (std::exception& e)
    {
        std::cerr << "error:AppStateMainMenu: " << e.what() << std::endl;
        throw;
    }

    // Setup resources
    gui->gui.setTarget(app.window.getWindowContext());

    buildLayout();

    auto onStateChanged = [this]
    {
        unsavedChanges = true;
        updateWindowTitle();
    };

    editor = mem::Box<Editor>(
        gui, canvas, onStateChanged, commandQueue, this->shortcutEngine);

    updateWindowTitle();
}

AppStateEditor::~AppStateEditor()
{
    settings->editorSettings.launchOptions =
        playtestLauncher->getLaunchParameters();
}

void AppStateEditor::buildLayout()
{
    const std::string TOPBAR_HEIGHT =
        std::to_string(Sizers::GetMenuBarHeight());
    const unsigned TOPBAR_FONT_HEIGHT = Sizers::GetMenuBarTextHeight();
    const std::string SIDEBAR_WIDTH = "8%";
    const std::string SIDEBAR_HEIGHT =
        "&.height - 2*" + TOPBAR_HEIGHT; // 2* because of the logger
    const std::string SIDEBAR_XPOS = "&.width - " + SIDEBAR_WIDTH;

    // Canvas
    auto runToken =
        buildCanvasLayout(SIDEBAR_WIDTH, SIDEBAR_HEIGHT, TOPBAR_HEIGHT);
    auto menuBar =
        buildMenuBarLayout(runToken, TOPBAR_HEIGHT, TOPBAR_FONT_HEIGHT);
    playtestLauncher->buildContextMenu(menuBar);
    buildSidebarLayout(runToken, SIDEBAR_WIDTH, SIDEBAR_HEIGHT, TOPBAR_HEIGHT);
    auto loggerChatBox =
        buildLoggerLayout(runToken, TOPBAR_HEIGHT, TOPBAR_FONT_HEIGHT);

    auto layerLabel = tgui::Label::create();
    layerLabel->setPosition({ "1%", ("100% - 2 * " + TOPBAR_HEIGHT).c_str() });
    gui->gui.add(layerLabel, "LayerLabel");
}

AppStateEditor::AllowExecutionToken AppStateEditor::buildCanvasLayout(
    const std::string& SIDEBAR_WIDTH,
    const std::string& SIDEBAR_HEIGHT,
    const std::string& TOPBAR_HEIGHT)
{
    canvas = tgui::CanvasSFML::create();
    canvas->setSize(
        ("100% - " + SIDEBAR_WIDTH).c_str(), SIDEBAR_HEIGHT.c_str());
    canvas->setPosition(0.f, TOPBAR_HEIGHT.c_str());
    gui->gui.add(canvas, "TilesetCanvas");
    return AllowExecutionToken();
}

tgui::MenuBar::Ptr AppStateEditor::buildMenuBarLayout(
    AllowExecutionToken,
    const std::string& TOPBAR_HEIGHT,
    unsigned TOPBAR_FONT_HEIGHT)
{
    auto menu = tgui::MenuBar::create();
    menu->setTextSize(TOPBAR_FONT_HEIGHT);
    menu->setRenderer(gui->theme.getRenderer("MenuBar"));
    menu->setSize("100%", TOPBAR_HEIGHT.c_str());
    menu->addMenu(Strings::AppState::CTX_MENU_NAME);

    auto addFileMenuItem = [this, &menu](
                               const std::string& label,
                               std::function<void(void)> callback,
                               std::optional<sf::Keyboard::Key> shortcut = {})
    {
        menu->addMenuItem(label);
        menu->connectMenuItem(
            Strings::AppState::CTX_MENU_NAME, label, callback);

        if (shortcut.has_value())
        {
            shortcutEngine->registerShortcut(
                "FileShortcuts", { true, false, shortcut.value() }, callback);
        }
    };

    using namespace Strings::AppState::ContextMenu;

    addFileMenuItem(
        NEW, [this] { handleNewLevel(); }, sf::Keyboard::N);
    addFileMenuItem(
        LOAD, [this] { handleLoadLevel(); }, sf::Keyboard::O);
    addFileMenuItem(
        SAVE, [this] { handleSaveLevel(); }, sf::Keyboard::S);
    addFileMenuItem(SAVE_AS, [this] { handleSaveLevel(true); });
    addFileMenuItem(
        UNDO, [this] { handleUndo(); }, sf::Keyboard::Z);
    addFileMenuItem(
        REDO, [this] { handleRedo(); }, sf::Keyboard::Y);
    addFileMenuItem(EXIT, [this] { handleExit(*dialogConfirmExit); });

    gui->gui.add(menu, "TopMenuBar");

    return menu;
}

void AppStateEditor::buildSidebarLayout(
    AllowExecutionToken,
    const std::string& SIDEBAR_WIDTH,
    const std::string& SIDEBAR_HEIGHT,
    const std::string& TOPBAR_HEIGHT)
{
    // only bootstrap the space it will be sitting in
    auto sidebar = tgui::Group::create();
    sidebar->setSize(SIDEBAR_WIDTH.c_str(), SIDEBAR_HEIGHT.c_str());
    sidebar->setPosition(
        ("100% - " + SIDEBAR_WIDTH).c_str(), TOPBAR_HEIGHT.c_str());
    gui->gui.add(sidebar, "Sidebar");
}

tgui::ChatBox::Ptr AppStateEditor::buildLoggerLayout(
    AllowExecutionToken, const std::string& TOPBAR_HEIGHT, unsigned)
{
    auto logger = tgui::ChatBox::create();
    logger->setRenderer(gui->theme.getRenderer("ChatBox"));
    logger->setSize("100%", TOPBAR_HEIGHT.c_str());
    logger->setPosition("0%", ("100% - " + TOPBAR_HEIGHT).c_str());
    logger->setTextSize(Sizers::GetMenuBarTextHeight());
    logger->setLinesStartFromTop();
    logger->setLineLimit(1);
    logger->addLine("This is a log console");
    gui->gui.add(logger, "LoggerBox");
    return logger;
}

void AppStateEditor::newLevelDialogCallback()
{
    savePath = "";
    updateWindowTitle();

    // Get settings from modal
    unsigned levelWidth = dialogNewLevel.getLevelWidth();
    unsigned levelHeight = dialogNewLevel.getLevelHeight();
    configPath = dialogNewLevel.getConfigPath();

    editor->init(levelWidth, levelHeight, configPath.value());
}

void AppStateEditor::handleNewLevel()
{
    dialogNewLevel.open([this] { newLevelDialogCallback(); });
}

void AppStateEditor::handleLoadLevel()
{
    auto r = fileApi->getOpenFileName(LVLD_FILTER);
    if (r.has_value()) loadLevel(r.value());
}

void AppStateEditor::loadLevel(
    const std::string& pathToLevel,
    std::optional<std::string> pathToConfigOverride)
{
    try
    {
        LevelD lvd;
        lvd.loadFromFile(pathToLevel);

        auto configPathFS = std::filesystem::path(
            pathToConfigOverride.value_or(lvd.metadata.description));
        if (!std::filesystem::exists(configPathFS))
        {
            dialogUpdateConfigPath.open(
                [&, pathToLevel] {
                    loadLevel(
                        pathToLevel, dialogUpdateConfigPath.getConfigPath());
                });
            dialogErrorInfo->open(
                "Path to config is invalid, provide a valid one");
            return; // abort this load, another one will trigger
        }

        savePath = pathToLevel;
        configPath = configPathFS.string();

        editor->loadFrom(lvd, configPathFS);
        unsavedChanges = false;
        updateWindowTitle();
    }
    catch (std::exception& e)
    {
        dialogErrorInfo->open(e.what());
    }
}

void AppStateEditor::handleSaveLevel(bool forceNewPath) noexcept
{
    assert(configPath.has_value());

    if (savePath.empty() || forceNewPath)
    {
        if (auto str = getNewSavePath())
            savePath = *str;
        else // user cancelled
            return;
    }

    try
    {
        unsavedChanges = false;
        auto&& lvd = editor->save();
        lvd.metadata.description = configPath.value();
        lvd.saveToFile(savePath);
        updateWindowTitle();
    }
    catch (std::exception& e)
    {
        dialogErrorInfo->open(e.what());
    }
}

void AppStateEditor::handleUndo()
{
    commandHistory->undo();
}

void AppStateEditor::handleRedo()
{
    commandHistory->redo();
}

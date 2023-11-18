#include "app/AppStateEditor.hpp"
#include "Configs/Sizers.hpp"
#include "Configs/Strings.hpp"
#include "Editor/Editor.hpp"
#include "Editor/NullEditor.hpp"
#include "Filesystem.hpp"
#include "Utilities/Literals.hpp"
#include "Utilities/ProcessCreator.hpp"
#include "app/AppStateIngame.hpp"
#include <cmath>

AppStateEditor::AppStateEditor(
    dgm::App& app,
    mem::Rc<tgui::Gui> nativeGui,
    mem::Rc<Gui> gui,
    mem::Rc<const dgm::ResourceManager> resmgr,
    mem::Rc<Settings> settings,
    mem::Rc<AudioPlayer> audioPlayer,
    mem::Rc<FileApiInterface> fileApi,
    mem::Rc<ShortcutEngineInterface> shortcutEngine,
    mem::Rc<YesNoCancelDialogInterface> dialogConfirmExit,
    mem::Rc<ErrorInfoDialogInterface> dialogErrorInfo)
    : dgm::AppState(app)
    , nativeGui(nativeGui)
    , gui(gui)
    , resmgr(resmgr)
    , settings(settings)
    , audioPlayer(audioPlayer)
    , fileApi(fileApi)
    , shortcutEngine(shortcutEngine)
    , dialogConfirmExit(dialogConfirmExit)
    , dialogErrorInfo(dialogErrorInfo)
    , editor(mem::Box<NullEditor>())
    , dialogNewLevel(gui, fileApi)
    , dialogLoadLevel(gui, settings->cmdSettings.resourcesDir)
    , dialogSaveLevel(gui)
    , dialogUpdateConfigPath(gui, fileApi)
{
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

void AppStateEditor::setupFont()
{
    gui->gui.setFont(resmgr->get<tgui::Font>("cruft.ttf").value().get());
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
            handleExit(*dialogConfirmExit, true);
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

AppStateEditor::~AppStateEditor() {}

void AppStateEditor::buildLayout()
{
    const std::string TOPBAR_HEIGHT =
        std::to_string(Sizers::GetMenuBarHeight());
    const unsigned TOPBAR_FONT_HEIGHT = Sizers::GetMenuBarTextHeight();
    const std::string SIDEBAR_WIDTH = "8%";
    const std::string SIDEBAR_HEIGHT = "&.height - " + TOPBAR_HEIGHT;
    const std::string SIDEBAR_XPOS = "&.width - " + SIDEBAR_WIDTH;

    // Canvas
    auto runToken =
        buildCanvasLayout(SIDEBAR_WIDTH, SIDEBAR_HEIGHT, TOPBAR_HEIGHT);
    auto menuBar =
        buildMenuBarLayout(runToken, TOPBAR_HEIGHT, TOPBAR_FONT_HEIGHT);
    buildSidebarLayout(runToken, SIDEBAR_WIDTH, SIDEBAR_HEIGHT, TOPBAR_HEIGHT);

    auto layerLabel = tgui::Label::create();
    layerLabel->setPosition({ "1%", ("100% - " + TOPBAR_HEIGHT).c_str() });
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
        PLAY, [this] { handlePlayLevel(); }, sf::Keyboard::F5);
    addFileMenuItem(
        UNDO, [this] { handleUndo(); }, sf::Keyboard::Z);
    addFileMenuItem(
        REDO, [this] { handleRedo(); }, sf::Keyboard::Y);
    addFileMenuItem(EXIT, [this] { handleExit(*dialogConfirmExit, false); });

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

void AppStateEditor::newLevelDialogCallback()
{
    savePath = "";
    updateWindowTitle();

    // Get settings from modal
    unsigned levelWidth = dialogNewLevel.getLevelWidth();
    unsigned levelHeight = dialogNewLevel.getLevelHeight();

    editor->init(
        levelWidth,
        levelHeight,
        Filesystem::getEditorConfigPath(settings->cmdSettings.resourcesDir));
}

void AppStateEditor::handleNewLevel()
{
    dialogNewLevel.open([this] { newLevelDialogCallback(); });
}

void AppStateEditor::handleLoadLevel()
{
    dialogLoadLevel.open(
        [this]
        {
            loadLevel(Filesystem::getFullLevelPath(
                          settings->cmdSettings.resourcesDir,
                          dialogLoadLevel.getLevelName())
                          .string());
        });
}

void AppStateEditor::loadLevel(
    const std::string& pathToLevel,
    std::optional<std::string> pathToConfigOverride)
{
    try
    {
        LevelD lvd;
        lvd.loadFromFile(pathToLevel);

        auto configPathFS =
            Filesystem::getEditorConfigPath(settings->cmdSettings.resourcesDir);
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
    if (savePath.empty() || forceNewPath)
    {
        dialogSaveLevel.open(
            [&]
            {
                savePath = Filesystem::getFullLevelPath(
                               settings->cmdSettings.resourcesDir,
                               dialogSaveLevel.getLevelName())
                               .string();
                saveLevel();
            });
    }
    else
    {
        saveLevel();
    }
}

void AppStateEditor::saveLevel()
{
    try
    {
        unsavedChanges = false;
        auto&& lvd = editor->save();
        lvd.saveToFile(savePath);
        updateWindowTitle();
    }
    catch (std::exception& e)
    {
        dialogErrorInfo->open(e.what());
    }
}

void AppStateEditor::handlePlayLevel()
{
    if (savePath.empty())
    {
        dialogErrorInfo->open(Strings::Dialog::Message::CANNOT_PLAY_LEVEL);
        return;
    }

    const auto gameSettings = GameSettings {
        .map = std::filesystem::path(savePath).filename().string(),
        .players = { PlayerSettings { .kind = PlayerKind::LocalHuman,
                                      .bindCamera = true } },
        .fraglimit = 1
    };

    auto lvd = LevelD {};
    lvd.loadFromFile(savePath);
    app.pushState<AppStateIngame>(
        resmgr,
        nativeGui,
        settings,
        audioPlayer,
        gameSettings,
        lvd,
        "launchedFromEditor"_true);
}

void AppStateEditor::handleUndo()
{
    commandHistory->undo();
}

void AppStateEditor::handleRedo()
{
    commandHistory->redo();
}

void AppStateEditor::handleExit(
    YesNoCancelDialogInterface& confirmExitDialog, bool exitApp)
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
                handleExit(confirmExitDialog, exitApp);
            });
    }
    else if (exitApp)
    {
        app.exit();
    }
    else
    {
        app.popState();
    }
}

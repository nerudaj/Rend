#include "app/AppStateEditor.hpp"
#include "Configs/Sizers.hpp"
#include "Configs/Strings.hpp"
#include "Editor/Editor.hpp"
#include "Editor/NullEditor.hpp"
#include "app/AppStateIngame.hpp"
#include <Filesystem.hpp>
#include <LevelMetadata.hpp>
#include <atomic>
#include <cmath>
#include <expected>

AppStateEditor::AppStateEditor(
    dgm::App& app,
    mem::Rc<DependencyContainer> dic,
    mem::Rc<ShortcutEngineInterface> shortcutEngine,
    mem::Rc<YesNoCancelDialogInterface> dialogConfirmExit,
    mem::Rc<ErrorInfoDialogInterface> dialogErrorInfo)
    : dgm::AppState(app)
    , dic(dic)
    , shortcutEngine(shortcutEngine)
    , dialogConfirmExit(dialogConfirmExit)
    , dialogErrorInfo(dialogErrorInfo)
    , editor(mem::Box<NullEditor>())
    , dialogNewLevel(dic->gui)
    , dialogLoadLevel(dic->gui, dic->settings->cmdSettings.resourcesDir)
    , dialogSaveLevel(
          dic->gui,
          Filesystem::getLevelsDir(dic->settings->cmdSettings.resourcesDir))
    , dialogLoading(dic->gui)
{
    dic->jukebox->stop();

    try
    {
        dic->gui->gui.setFont(
            dic->resmgr->get<tgui::Font>("cruft.ttf").value().get());
    }
    catch (std::exception& e)
    {
        std::cerr << "error:AppStateMainMenu: " << e.what() << std::endl;
        throw;
    }

    buildLayout();

    updateWindowTitle();
}

AppStateEditor::~AppStateEditor()
{
    dic->gui->gui.setFont(
        dic->resmgr->get<tgui::Font>("pico-8.ttf").value().get());
}

mem::Box<Editor>
AppStateEditor::startEditor(unsigned levelWidth, unsigned levelHeight)
{
    {
        auto level = LevelD();
        level.mesh.layerWidth = levelWidth;
        level.mesh.layerHeight = levelHeight;
        level.mesh.tileWidth = 16u;
        level.mesh.tileHeight = 16u;
        level.mesh.layers.resize(2);
        level.mesh.layers[0].blocks =
            std::vector<bool>(levelWidth * levelHeight, false);
        level.mesh.layers[0].tiles = std::vector<uint16_t>(
            levelWidth * levelHeight,
            static_cast<uint16_t>(LevelTileId::Flat1));
        level.mesh.layers[1].blocks =
            std::vector<bool>(levelWidth * levelHeight, false);
        level.mesh.layers[1].tiles = std::vector<uint16_t>(
            levelWidth * levelHeight,
            static_cast<uint16_t>(LevelTileId::CeilSky));
        return startEditor(level);
    }
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

        try
        {
            dic->gui->gui.handleEvent(event);
            editor->handleEvent(event, mousePos);
            shortcutEngine->handleEvent(event);
        }
        catch (std::exception& e)
        {
            dialogErrorInfo->open(e.what());
        }
    }
}

void AppStateEditor::update()
{
    clickPreventer.update();

    while (!delayedActions.empty() && delayActionsForNumFrames == 0)
    {
        delayedActions.top()();
        delayedActions.pop();
    }

    if (delayActionsForNumFrames > 0) delayActionsForNumFrames--;

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
    dic->gui->draw();
}

void AppStateEditor::buildLayout()
{
    const std::string TOPBAR_HEIGHT =
        std::to_string(Sizers::GetMenuBarHeight());
    const unsigned TOPBAR_FONT_HEIGHT = Sizers::GetMenuBarTextHeight();
    const std::string SIDEBAR_WIDTH = "8%";
    const std::string SIDEBAR_HEIGHT = "&.height - " + TOPBAR_HEIGHT;
    const std::string SIDEBAR_XPOS = "&.width - " + SIDEBAR_WIDTH;

    dic->gui->removeAllWidgets();

    // Canvas
    auto runToken =
        buildCanvasLayout(SIDEBAR_WIDTH, SIDEBAR_HEIGHT, TOPBAR_HEIGHT);
    auto menuBar =
        buildMenuBarLayout(runToken, TOPBAR_HEIGHT, TOPBAR_FONT_HEIGHT);
    buildSidebarLayout(runToken, SIDEBAR_WIDTH, SIDEBAR_HEIGHT, TOPBAR_HEIGHT);
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
    dic->gui->add(canvas, "TilesetCanvas");
    return AllowExecutionToken();
}

tgui::MenuBar::Ptr AppStateEditor::buildMenuBarLayout(
    AllowExecutionToken,
    const std::string& TOPBAR_HEIGHT,
    unsigned TOPBAR_FONT_HEIGHT)
{
    auto menu = tgui::MenuBar::create();
    menu->setTextSize(TOPBAR_FONT_HEIGHT);
    menu->setRenderer(dic->gui->theme->getRenderer("MenuBar"));
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
        PLAY,
        [this] { handlePlayLevelWrapper("useBot"_false); },
        sf::Keyboard::F5);
    addFileMenuItem(PLAY2P, [this] { handlePlayLevelWrapper("useBot"_true); });
    addFileMenuItem(
        UNDO, [this] { handleUndo(); }, sf::Keyboard::Z);
    addFileMenuItem(
        REDO, [this] { handleRedo(); }, sf::Keyboard::Y);
    addFileMenuItem(EXIT, [this] { handleExit(*dialogConfirmExit, false); });

    dic->gui->gui.add(menu, "TopMenuBar");

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
    dic->gui->add(sidebar, "Sidebar");
}

void AppStateEditor::newLevelDialogCallback()
{
    savePath = "";
    updateWindowTitle();

    // Get settings from modal
    unsigned levelWidth = dialogNewLevel.getLevelWidth();
    unsigned levelHeight = dialogNewLevel.getLevelHeight();
    levelMetadata->author = dialogNewLevel.getAuthorName();
    levelMetadata->skyboxTheme = dialogNewLevel.getSkyboxTheme();
    levelMetadata->texturePack = dialogNewLevel.getTexturePack();

    editor = startEditor(levelWidth, levelHeight);
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
                          dic->settings->cmdSettings.resourcesDir,
                          dialogLoadLevel.getMapPackName(),
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

        savePath = pathToLevel;

        levelMetadata->author = lvd.metadata.author;
        auto theme = LevelTheme::fromJson(lvd.metadata.description);
        levelMetadata->skyboxTheme = SkyboxThemeUtils::fromString(theme.skybox);
        levelMetadata->texturePack =
            TexturePackUtils::fromString(theme.textures);

        editor = startEditor(lvd);
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
                               dic->settings->cmdSettings.resourcesDir,
                               dialogSaveLevel.getMapPackName(),
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
        lvd.metadata.author = levelMetadata->author;
        lvd.metadata.description =
            nlohmann::json(LevelTheme { .skybox = SkyboxThemeUtils::toString(
                                            levelMetadata->skyboxTheme),
                                        .textures = TexturePackUtils::toString(
                                            levelMetadata->texturePack) })
                .dump();

        if (!std::filesystem::exists(savePath.parent_path()))
        {
            std::filesystem::create_directory(savePath.parent_path());
        }

        lvd.saveToFile(savePath.string());
        updateWindowTitle();
    }
    catch (std::exception& e)
    {
        dialogErrorInfo->open(e.what());
    }
}

void AppStateEditor::handlePlayLevelWrapper(bool useBot)
{
    dialogLoading.open(Strings::Dialog::Message::COMPUTING_DISTANCE_INDEX);
    delayedActions.push([this, useBot] { handlePlayLevel(useBot); });
    shortcutEngine->releaseSpecialKeys();
    delayActionsForNumFrames = 1;
}

void AppStateEditor::handlePlayLevel(bool useBot)
{
    if (savePath.empty())
    {
        dialogErrorInfo->open(Strings::Dialog::Message::CANNOT_PLAY_LEVEL);
        return;
    }

    try
    {
        handleSaveLevel();

        auto gameSettings = GameOptions { .players = { PlayerOptions {
                                              .kind = PlayerKind::LocalHuman,
                                              .bindCamera = true } },
                                          .pointlimit = 99 };
        if (useBot)
        {
            gameSettings.players.push_back(PlayerOptions {
                .kind = PlayerKind::LocalNpc, .bindCamera = false });
        }

        auto lvd = LevelD {};
        lvd.loadFromFile(savePath.string());

        auto&& client = mem::Rc<Client>("127.0.0.1", 10666ui16);
        if (auto&& result = client->sendPeerReadySignal(); !result)
            throw std::runtime_error(result.error());

        client->readIncomingPackets([](auto) {});

        app.pushState<AppStateIngame>(
            dic, client, gameSettings, lvd, "launchedFromEditor"_true);
    }
    catch (const std::exception& e)
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

void AppStateEditor::handleExit(
    YesNoCancelDialogInterface& confirmExitDialog, bool exitApp)
{
    if (unsavedChanges)
    {
        using namespace Strings::Dialog;

        confirmExitDialog.open(
            Title::WARNING,
            Message::UNSAVED_CHANGES,
            [this, exitApp, &confirmExitDialog](UserChoice choice)
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

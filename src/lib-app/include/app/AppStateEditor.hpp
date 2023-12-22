#pragma once

#include "Camera.hpp"
#include "Commands/CommandHistory.hpp"
#include "Commands/CommandQueue.hpp"
#include "Dialogs/LoadLevelDialog.hpp"
#include "Dialogs/NewLevelDialog.hpp"
#include "Dialogs/SaveLevelDialog.hpp"
#include "Editor/Editor.hpp"
#include "Gui.hpp"
#include "Interfaces/DialogInterfaces.hpp"
#include "Interfaces/EditorInterface.hpp"
#include "Interfaces/PlaytestLauncherInterface.hpp"
#include "Interfaces/ShortcutEngineInterface.hpp"
#include "Utilities/ClickPreventer.hpp"
#include <DGM/dgm.hpp>
#include <LevelMetadata.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <optional>

import Memory;
import Options;
import Audio;
import Input;

class AppStateEditor : public dgm::AppState
{
public:
    AppStateEditor(
        dgm::App& app,
        mem::Rc<tgui::Gui> nativeGui,
        mem::Rc<Gui> gui,
        mem::Rc<const dgm::ResourceManager> resmgr,
        mem::Rc<AppOptions> settings,
        mem::Rc<AudioPlayer> audioPlayer,
        mem::Rc<Jukebox> jukebox,
        mem::Rc<PhysicalController> controller,
        mem::Rc<ShortcutEngineInterface> shortcutEngine,
        mem::Rc<YesNoCancelDialogInterface> dialogConfirmExit,
        mem::Rc<ErrorInfoDialogInterface> dialogErrorInfo);
    ~AppStateEditor();

public:
    // Inherited via AppState
    virtual void input() override;
    virtual void update() override;
    virtual void draw() override;

    virtual [[nodiscard]] bool isTransparent() const noexcept override
    {
        return false;
    }

    virtual void restoreFocus()
    {
        jukebox->stop();
    }

protected:
    void updateWindowTitle()
    {
        app.window.getWindowContext().setTitle(
            "Rend Level Editor" + (savePath.empty() ? "" : " - " + savePath)
            + (unsavedChanges ? " *" : ""));
    }

    // IO
    void newLevelDialogCallback();

protected: // Build functions
    class AllowExecutionToken;

    void buildLayout();
    AllowExecutionToken buildCanvasLayout(
        const std::string& SIDEBAR_WIDTH,
        const std::string& SIDEBAR_HEIGHT,
        const std::string& TOPBAR_HEIGHT);

    class AllowExecutionToken
    {
        AllowExecutionToken() = default;
        friend AllowExecutionToken AppStateEditor::buildCanvasLayout(
            const std::string&, const std::string&, const std::string&);
    };

    tgui::MenuBar::Ptr buildMenuBarLayout(
        AllowExecutionToken,
        const std::string& TOPBAR_HEIGHT,
        unsigned TOPBAR_FONT_HEIGHT);
    void buildSidebarLayout(
        AllowExecutionToken,
        const std::string& SIDEBAR_WIDTH,
        const std::string& SIDEBAR_HEIGHT,
        const std::string& TOPBAR_HEIGHT);

protected: // Callback handlers
    void handleNewLevel();
    void handleLoadLevel();
    void loadLevel(
        const std::string& pathToLevel,
        std::optional<std::string> pathToConfigOverride = {});
    void handleSaveLevel(bool forceNewPath = false) noexcept;
    void saveLevel();
    void handlePlayLevel();
    void handleUndo();
    void handleRedo();
    void handleExit(YesNoCancelDialogInterface& dialoConfirmExit, bool exitApp);

protected:
    void setupFont();

    [[nodiscard]] mem::Box<Editor>
    startEditor(unsigned levelWidth, unsigned levelHeight);

    [[nodiscard]] mem::Box<Editor> startEditor(const LevelD& level)
    {
        return mem::Box<Editor>(
            gui,
            canvas,
            onStateChanged,
            commandQueue,
            shortcutEngine,
            levelMetadata,
            level,
            settings->cmdSettings.resourcesDir / "graphics");
    }

protected:
    mem::Rc<tgui::Gui> nativeGui;
    mem::Rc<Gui> gui;
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<AppOptions> settings;
    mem::Rc<AudioPlayer> audioPlayer;
    mem::Rc<Jukebox> jukebox;
    mem::Rc<PhysicalController> controller;
    mem::Rc<ShortcutEngineInterface> shortcutEngine;
    mem::Rc<YesNoCancelDialogInterface> dialogConfirmExit;
    mem::Rc<ErrorInfoDialogInterface> dialogErrorInfo;

    // Attributes
    std::string savePath;
    bool unsavedChanges = false;
    tgui::CanvasSFML::Ptr canvas;
    mem::Rc<CommandHistory> commandHistory;
    mem::Rc<CommandQueue> commandQueue = mem::Rc<CommandQueue>(commandHistory);
    mem::Box<EditorInterface> editor;
    ModernNewLevelDialog dialogNewLevel;
    LoadLevelDialog dialogLoadLevel;
    SaveLevelDialog dialogSaveLevel;
    ClickPreventer clickPreventer;
    mem::Rc<LevelMetadata> levelMetadata;

    std::function<void(void)> onStateChanged = [this]
    {
        unsavedChanges = true;
        updateWindowTitle();
    };
};

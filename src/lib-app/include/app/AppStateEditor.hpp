#pragma once

#include "Camera.hpp"
#include "Commands/CommandHistory.hpp"
#include "Commands/CommandQueue.hpp"
#include "Dialogs/LoadLevelDialog.hpp"
#include "Dialogs/NewLevelDialog.hpp"
#include "Dialogs/SaveLevelDialog.hpp"
#include "Dialogs/UpdateConfigPathDialog.hpp"
#include "Gui.hpp"
#include "Interfaces/DialogInterfaces.hpp"
#include "Interfaces/EditorInterface.hpp"
#include "Interfaces/FileApiInterface.hpp"
#include "Interfaces/PlaytestLauncherInterface.hpp"
#include "Interfaces/ShortcutEngineInterface.hpp"
#include "Utilities/ClickPreventer.hpp"
#include <DGM/dgm.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <optional>

import Memory;
import Options;
import Audio;

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
        mem::Rc<FileApiInterface> fileApi,
        mem::Rc<ShortcutEngineInterface> shortcutEngine,
        mem::Rc<YesNoCancelDialogInterface> dialogConfirmExit,
        mem::Rc<ErrorInfoDialogInterface> dialogErrorInfo);
    ~AppStateEditor();

protected:
    mem::Rc<tgui::Gui> nativeGui;
    mem::Rc<Gui> gui;
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<AppOptions> settings;
    mem::Rc<AudioPlayer> audioPlayer;
    mem::Rc<ShortcutEngineInterface> shortcutEngine;
    mem::Rc<FileApiInterface> fileApi;
    mem::Rc<YesNoCancelDialogInterface> dialogConfirmExit;
    mem::Rc<ErrorInfoDialogInterface> dialogErrorInfo;

    // Attributes
    std::string savePath;
    bool unsavedChanges = false;
    tgui::CanvasSFML::Ptr canvas;
    mem::Rc<CommandHistory> commandHistory;
    mem::Rc<CommandQueue> commandQueue = mem::Rc<CommandQueue>(commandHistory);
    mem::Box<EditorInterface> editor;
    NewLevelDialog dialogNewLevel;
    LoadLevelDialog dialogLoadLevel;
    SaveLevelDialog dialogSaveLevel;
    UpdateConfigPathDialog dialogUpdateConfigPath;
    ClickPreventer clickPreventer;

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

public:
    // Inherited via AppState
    virtual void input() override;
    virtual void update() override;
    virtual void draw() override;

    virtual [[nodiscard]] bool isTransparent() const noexcept override
    {
        return false;
    }
};

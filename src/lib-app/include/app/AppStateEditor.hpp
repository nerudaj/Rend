#pragma once

#include "Camera.hpp"
#include "Commands/CommandHistory.hpp"
#include "Commands/CommandQueue.hpp"
#include "Dialogs/NewLevelDialog.hpp"
#include "Dialogs/UpdateConfigPathDialog.hpp"
#include "Gui.hpp"
#include "Interfaces/DialogInterfaces.hpp"
#include "Interfaces/EditorInterface.hpp"
#include "Interfaces/FileApiInterface.hpp"
#include "Interfaces/PlaytestLauncherInterface.hpp"
#include "Interfaces/ShortcutEngineInterface.hpp"
#include "Utilities/ClickPreventer.hpp"
#include <DGM/dgm.hpp>
#include <Settings.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <optional>

import Memory;

class AppStateEditor : public dgm::AppState
{
public:
    AppStateEditor(
        dgm::App& app,
        mem::Rc<Gui> gui,
        mem::Rc<const dgm::ResourceManager> resmgr,
        mem::Rc<Settings> settings,
        mem::Rc<FileApiInterface> fileApi,
        mem::Rc<ShortcutEngineInterface> shortcutEngine,
        mem::Rc<YesNoCancelDialogInterface> dialogConfirmExit,
        mem::Rc<ErrorInfoDialogInterface> dialogErrorInfo);
    ~AppStateEditor();

protected:
    mem::Rc<Gui> gui;
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<Settings> settings;
    mem::Rc<ShortcutEngineInterface> shortcutEngine;
    mem::Rc<FileApiInterface> fileApi;
    mem::Rc<YesNoCancelDialogInterface> dialogConfirmExit;
    mem::Rc<ErrorInfoDialogInterface> dialogErrorInfo;

    // Attributes
    std::string savePath;
    std::optional<std::string> configPath = {};
    bool unsavedChanges = false;
    tgui::CanvasSFML::Ptr canvas;
    mem::Rc<CommandHistory> commandHistory;
    mem::Rc<CommandQueue> commandQueue = mem::Rc<CommandQueue>(commandHistory);
    mem::Box<EditorInterface> editor;
    NewLevelDialog dialogNewLevel;
    UpdateConfigPathDialog dialogUpdateConfigPath;
    ClickPreventer clickPreventer;
    mem::Box<PlaytestLauncherInterface> playtestLauncher;

protected:
    void updateWindowTitle()
    {
        app.window.getWindowContext().setTitle(
            "DGM Level Editor" + (savePath.empty() ? "" : " - " + savePath)
            + (unsavedChanges ? " *" : ""));
    }

    // IO
    void newLevelDialogCallback();
    std::optional<std::string> getNewSavePath();

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
    tgui::ChatBox::Ptr buildLoggerLayout(
        AllowExecutionToken,
        const std::string& TOPBAR_HEIGHT,
        unsigned TOPBAR_FONT_HEIGHT);

protected: // Callback handlers
    void handleNewLevel();
    void handleLoadLevel();
    void loadLevel(
        const std::string& pathToLevel,
        std::optional<std::string> pathToConfigOverride = {});
    void handleSaveLevel(bool forceNewPath = false) noexcept;
    void handleUndo();
    void handleRedo();
    void handleExit(YesNoCancelDialogInterface& dialoConfirmExit);

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

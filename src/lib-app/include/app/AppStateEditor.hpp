#pragma once

#include "Camera.hpp"
#include "Commands/CommandHistory.hpp"
#include "Commands/CommandQueue.hpp"
#include "Dialogs/LoadLevelDialog.hpp"
#include "Dialogs/LoadingDialog.hpp"
#include "Dialogs/NewLevelDialog.hpp"
#include "Dialogs/SaveLevelDialog.hpp"
#include "Editor/Editor.hpp"
#include "Interfaces/DialogInterfaces.hpp"
#include "Interfaces/EditorInterface.hpp"
#include "Interfaces/PlaytestLauncherInterface.hpp"
#include "Interfaces/ShortcutEngineInterface.hpp"
#include "Utilities/ClickPreventer.hpp"
#include "utils/DependencyContainer.hpp"
#include <DGM/dgm.hpp>
#include <LevelMetadata.hpp>
#include <Memory.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <optional>
#include <stack>

class [[nodiscard]] AppStateEditor final : public dgm::AppState
{
public:
    AppStateEditor(
        dgm::App& app,
        mem::Rc<DependencyContainer> dic,
        mem::Rc<ShortcutEngineInterface> shortcutEngine,
        mem::Rc<YesNoCancelDialogInterface> dialogConfirmExit,
        mem::Rc<ErrorInfoDialogInterface> dialogErrorInfo);
    ~AppStateEditor();

public:
    // Inherited via AppState
    virtual void input() override;
    virtual void update() override;
    virtual void draw() override;

protected:
    virtual void restoreFocusImpl(const std::string&)
    {
        dic->jukebox->stop();
        dialogLoading.close();
    }

    void updateWindowTitle()
    {
        app.window.getWindowContext().setTitle(
            "Rend Level Editor"
            + (savePath.empty() ? "" : " - " + savePath.string())
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
    void handlePlayLevelWrapper(bool useBot);
    void handlePlayLevel(bool useBot);
    void handleUndo();
    void handleRedo();
    void handleExit(YesNoCancelDialogInterface& dialoConfirmExit, bool exitApp);

protected:
    [[nodiscard]] mem::Box<Editor>
    startEditor(unsigned levelWidth, unsigned levelHeight);

    [[nodiscard]] mem::Box<Editor> startEditor(const LevelD& level)
    {
        return mem::Box<Editor>(
            dic->gui,
            canvas,
            onStateChanged,
            commandQueue,
            shortcutEngine,
            levelMetadata,
            level,
            dic->settings->cmdSettings.resourcesDir / "graphics");
    }

protected:
    mem::Rc<DependencyContainer> dic;
    mem::Rc<ShortcutEngineInterface> shortcutEngine;
    mem::Rc<YesNoCancelDialogInterface> dialogConfirmExit;
    mem::Rc<ErrorInfoDialogInterface> dialogErrorInfo;

    // Attributes
    std::filesystem::path savePath;
    bool unsavedChanges = false;
    tgui::CanvasSFML::Ptr canvas;
    mem::Rc<CommandHistory> commandHistory;
    mem::Rc<CommandQueue> commandQueue = mem::Rc<CommandQueue>(commandHistory);
    mem::Box<EditorInterface> editor;
    ModernNewLevelDialog dialogNewLevel;
    LoadLevelDialog dialogLoadLevel;
    NewSaveLevelDialog dialogSaveLevel;
    LoadingDialog dialogLoading;
    ClickPreventer clickPreventer;
    mem::Rc<LevelMetadata> levelMetadata;
    std::stack<std::function<void(void)>> delayedActions;
    int delayActionsForNumFrames = 0;

    std::function<void(void)> onStateChanged = [this]
    {
        unsavedChanges = true;
        updateWindowTitle();
    };
};

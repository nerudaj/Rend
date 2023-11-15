#include "Stubs/FileApiStub.hpp"
#include "Stubs/YesNoCancelDialogStub.hpp"
#include "TestHelpers/EditorMock.hpp"
#include "TestHelpers/NullCallback.hpp"
#include "include/Dialogs/ErrorInfoDialog.hpp"
#include <catch.hpp>
#include <fakeit.hpp>
#include <filesystem>
#include <include/AppStateEditor.hpp>
#include <include/Shortcuts/ShortcutEngine.hpp>

using namespace fakeit;

class AppStateEditorTestable : public AppStateEditor
{
public:
    std::optional<std::string> getNewSavePathTest()
    {
        return getNewSavePath();
    }

    void mockUnsavedChanges()
    {
        configPath = "dummy";
        unsavedChanges = true;
    }

    void handleExitTest()
    {
        handleExit(*dialogConfirmExit);
    }

    void injectEditorMock(Box<EditorInterface> mock)
    {
        editor = std::move(mock);
    }

    AppStateEditorTestable(
        dgm::App& app,
        GC<Gui> gui,
        cfg::Ini& ini,
        ProgramOptions options,
        GC<FileApiInterface> fileApi,
        GC<ShortcutEngineInterface> shortcutEngine,
        GC<YesNoCancelDialogInterface> yesNoDialog,
        GC<ErrorInfoDialogInterface> errorDialog)
        : AppStateEditor(
            app,
            gui,
            ini,
            options,
            fileApi,
            shortcutEngine,
            yesNoDialog,
            errorDialog)
    {
    }
};

class DgmAppTestable : public dgm::App
{
public:
    bool exitCalled() const
    {
        return numberOfStatesToPop > 0;
    }

    AppStateEditorTestable& getState()
    {
        return dynamic_cast<AppStateEditorTestable&>(topState());
    }

    DgmAppTestable(dgm::Window& window) : dgm::App(window) {}
};

static std::string getRootPath()
{
    auto path = std::filesystem::current_path();
    return (path / ".." / ".." / "..").generic_string();
}

TEST_CASE("[AppStateEditor]")
{
    dgm::Window window;
    cfg::Ini
        ini; // Ini must be declared before app to outlive it during destruction
    DgmAppTestable app(window);
    ProgramOptions options {
        .binaryDir = "C:\\dummy",
        .rootDir = getRootPath(),
        .binaryDirHash = "Testing",
    };

    auto fileApiMock = GC<FileApiStub>();
    auto fileApiSpy = Mock<FileApiInterface>(*fileApiMock);

    auto yesNoDialogMock = GC<YesNoCancelDialogStub>();
    auto yesNoDialogSpy = Mock<YesNoCancelDialogInterface>(*yesNoDialogMock);

    auto shortcutEngine = GC<ShortcutEngine>([] { return false; });
    EditorMockState editorMockState;
    auto&& gui = GC<Gui>();
    GC<ErrorInfoDialogInterface> errorInfoDialog = GC<ErrorInfoDialog>(gui);

    SECTION("handleExit")
    {
        app.pushState<AppStateEditorTestable>(
            gui,
            ini,
            options,
            fileApiMock,
            shortcutEngine,
            yesNoDialogMock,
            errorInfoDialog);

        SECTION("Asks for confirmation with unsaved changes")
        {
            app.getState().injectEditorMock(Box<EditorMock>(&editorMockState));
            app.getState().mockUnsavedChanges();

            When(Method(yesNoDialogSpy, open))
                .Do([](const std::string,
                       const std::string&,
                       std::function<void(UserChoice)> completedCallback)
                    { completedCallback(UserChoice::Confirmed); });

            app.getState().handleExitTest();

            REQUIRE(app.exitCalled());
            REQUIRE(editorMockState.saveToFileCallCounter == 1);
        }

        SECTION("Does not save if user rejects")
        {
            app.getState().injectEditorMock(Box<EditorMock>(&editorMockState));
            app.getState().mockUnsavedChanges();

            When(Method(yesNoDialogSpy, open))
                .Do([](const std::string,
                       const std::string&,
                       std::function<void(UserChoice)> completedCallback)
                    { completedCallback(UserChoice::Denied); });

            app.getState().handleExitTest();

            REQUIRE(app.exitCalled());
            REQUIRE(editorMockState.saveToFileCallCounter == 0);
        }

        SECTION("Does not exit if user cancels")
        {
            app.getState().injectEditorMock(Box<EditorMock>(&editorMockState));
            app.getState().mockUnsavedChanges();

            When(Method(yesNoDialogSpy, open))
                .Do([](const std::string,
                       const std::string&,
                       std::function<void(UserChoice)> completedCallback)
                    { completedCallback(UserChoice::Cancelled); });

            app.getState().handleExitTest();

            REQUIRE(not app.exitCalled());
            REQUIRE(editorMockState.saveToFileCallCounter == 0);
        }
    }

    SECTION("getNewSavePath")
    {
        auto appStateEditor = AppStateEditorTestable(
            app,
            gui,
            ini,
            options,
            fileApiMock,
            shortcutEngine,
            yesNoDialogMock,
            errorInfoDialog);

        SECTION("Returns nullopt on user cancel")
        {
            When(Method(fileApiSpy, getSaveFileName))
                .Do([](const char*) -> std::optional<std::string>
                    { return {}; });

            REQUIRE_FALSE(appStateEditor.getNewSavePathTest().has_value());
        }

        SECTION("Appends .lvd if not specified")
        {
            When(Method(fileApiSpy, getSaveFileName))
                .Do([](const char*) -> std::optional<std::string>
                    { return "name"; });

            REQUIRE(appStateEditor.getNewSavePathTest().value() == "name.lvd");
        }

        SECTION("Does nothing if .lvd is specified")
        {
            When(Method(fileApiSpy, getSaveFileName))
                .Do([](const char*) -> std::optional<std::string>
                    { return "name.lvd"; });

            REQUIRE(appStateEditor.getNewSavePathTest().value() == "name.lvd");
        }
    }
}

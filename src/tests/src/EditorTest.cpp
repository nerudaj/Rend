#include "TestHelpers/LeveldBuilder.hpp"
#include "TestHelpers/NullCallback.hpp"
#include "TestHelpers/TestAssets.hpp"
#include <Editor/Editor.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <catch.hpp>
#include <fakeit.hpp>

import Memory;

using namespace fakeit;

TEST_CASE("[Editor]")
{
    mem::Rc<Gui> gui;
    gui->gui.add(tgui::MenuBar::create(), "TopMenuBar");
    gui->gui.add(tgui::Group::create(), "Sidebar");

    sf::View view;
    tgui::CanvasSFML::Ptr canvas = tgui::CanvasSFML::create();
    canvas->setView(view);
    CommandHistory commandHistory;
    mem::Rc<CommandQueue> commandQueue(commandHistory);
    auto&& shortcutEngine = mem::Rc<ShortcutEngine> { [] { return false; } };
    mem::Rc<LevelMetadata> metadata;

    SECTION("onStateChanged")
    {
        bool stateChanged = false;
        auto&& onStateChangedMock = [&stateChanged] { stateChanged = true; };

        auto&& editor = Editor(
            gui,
            canvas,
            onStateChangedMock,
            commandQueue,
            shortcutEngine,
            metadata,
            LevelD {},
            GRAPHICS_DIR);

        SECTION("When metadata are changed, callback is triggered")
        {
            // editMetadataDialog will contain default values that are different
            // from these
            metadata->author = "test";
            editor.handleChangedMetadata();
            REQUIRE(stateChanged);
        }

        SECTION("Callback is not triggered when metadata are not changed")
        {
            editor.handleChangedMetadata();
            REQUIRE_FALSE(stateChanged);
        }
    }
}

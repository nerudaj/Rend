#include "TestHelpers/LeveldBuilder.hpp"
#include "TestHelpers/NullCallback.hpp"
#include "TestHelpers/TestAssets.hpp"
#include <catch.hpp>
#include <include/Editor/Editor.hpp>

TEST_CASE("[Editor]")
{
    GC<Gui> gui;
    gui->gui.add(tgui::MenuBar::create(), "TopMenuBar");
    gui->gui.add(tgui::Group::create(), "Sidebar");

    sf::View view;
    tgui::Canvas::Ptr canvas = tgui::Canvas::create();
    canvas->setView(view);
    CommandHistory commandHistory;
    GC<CommandQueue> commandQueue(commandHistory);
    auto&& shortcutEngine = GC<ShortcutEngine> { [] { return false; } };

    Editor editor(gui, canvas, Null::Callback, commandQueue, shortcutEngine);

    SECTION("loadFrom")
    {
        SECTION("Can load another level when one was already loaded")
        {
            LevelD level1 = LeveldBuilder::buildWithMesh(32, 20, {});
            LevelD level2 = LeveldBuilder::buildWithMesh(40, 40, {});

            editor.loadFrom(level1, Config::CONFIG_PATH);
            editor.loadFrom(level2, Config::CONFIG_PATH);
        }
    }
}

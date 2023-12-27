#include "TestHelpers/LeveldBuilder.hpp"
#include "TestHelpers/NullCallback.hpp"
#include "TestHelpers/TestAssets.hpp"
#include <Editor/Editor.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <catch.hpp>

import Memory;

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

    // No tests right now
}

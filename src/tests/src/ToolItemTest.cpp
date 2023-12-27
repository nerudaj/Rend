#include "Shortcuts/ShortcutEngine.hpp"
#include "TestHelpers/LeveldBuilder.hpp"
#include "TestHelpers/TestAssets.hpp"
#include "Tools/LayerController.hpp"
#include <TestHelpers/NullCallback.hpp>
#include <Tools/ToolItem.hpp>
#include <catch.hpp>

import Memory;

LevelD createLevelWithSpawns(unsigned spawnCount)
{
    LevelD lvd;
    for (unsigned i = 0; i < spawnCount; i++)
        lvd.things.push_back(LevelD::Thing {
            .id = static_cast<uint32_t>(LevelItemId::PlayerSpawn) });
    return lvd;
}

TEST_CASE("[ToolItem]")
{
    auto&& shortcutEngine = mem::Rc<ShortcutEngine> { [] { return false; } };
    mem::Rc<CommandHistory> commandHistory;
    mem::Rc<CommandQueue> commandQueue(commandHistory);
    mem::Rc<Gui> gui;
    mem::Rc<LayerController> layerController(gui->gui);

    auto makeLevel = [&](const LevelD& level)
    {
        return ToolItem(
            Null::Callback,
            shortcutEngine,
            layerController,
            gui,
            commandQueue,
            Mesh::MESH_TEXTURE_PATH,
            Item::ITEM_CLIP,
            level);
    };

    SECTION("On save validation")
    {
        SECTION("Throws when there are no spawns")
        {
            auto&& tool = makeLevel(LevelD {});
            REQUIRE_THROWS([&] { tool.validateBeforeSave(); }());
        }

        SECTION("Throws when there are less than four spawns")
        {

            for (unsigned i = 1; i <= 3; ++i)
            {
                auto&& tool = makeLevel(createLevelWithSpawns(i));
                REQUIRE_THROWS([&] { tool.validateBeforeSave(); }());
            }
        }

        SECTION("Doesn't throw on four or more spawns")
        {
            for (unsigned i = 4; i <= 6; ++i)
            {
                auto&& tool = makeLevel(createLevelWithSpawns(i));
                REQUIRE_NOTHROW([&] { tool.validateBeforeSave(); }());
            }
        }
    }
}

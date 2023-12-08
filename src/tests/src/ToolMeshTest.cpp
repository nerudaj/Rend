#include "Tools/ToolMesh.hpp"
#include "Shortcuts/ShortcutEngine.hpp"
#include "TestHelpers/LeveldBuilder.hpp"
#include "TestHelpers/NullCallback.hpp"
#include "TestHelpers/TestAssets.hpp"
#include "Tools/LayerController.hpp"
#include <catch.hpp>

import Memory;

TEST_CASE("[ToolMesh]")
{
    auto&& shortcutEngine = mem::Rc<ShortcutEngine> { [] { return false; } };
    mem::Rc<CommandHistory> commandHistory;
    mem::Rc<CommandQueue> commandQueue(commandHistory);
    mem::Rc<LayerController> layerController;
    mem::Rc<Gui> gui;

    ToolMesh mesh(
        Null::Callback, shortcutEngine, layerController, gui, commandQueue);

    mesh.configure(
        Mesh::MESH_TEXTURE_PATH,
        Mesh::FRAME_SIZE,
        Mesh::FRAME_SPACING,
        Mesh::TEXTURE_BOUNDS,
        { false, true, false, true });

    SECTION(
        "Adds extra layers if source object doesn't have the correct amount")
    {
        LevelD level = LeveldBuilder::buildWithMesh(20, 20, {});
        mesh.loadFrom(level);

        LevelD exported;
        mesh.saveTo(exported);

        REQUIRE(exported.mesh.layers.size() == 2u);
    }

    SECTION("getBoundingBox")
    {
        SECTION("returns empty box, when all tiles are empty")
        {
            LevelD level = LeveldBuilder::buildWithMesh(20, 20, {});
            mesh.loadFrom(level);
            REQUIRE(!mesh.getBoundingBox().has_value());
        }

        SECTION("returns valid box, when some tiles are set")
        {
            LevelD level = LeveldBuilder::buildWithMesh(
                20, 20, { { 5, 5, 1, 1 }, { 10, 10, 2, 0 } });

            mesh.loadFrom(level);

            auto box = mesh.getBoundingBox();

            REQUIRE(box.has_value());
            REQUIRE(box.value().left == 5u);
            REQUIRE(box.value().top == 5u);
            REQUIRE(box.value().right == 10u);
            REQUIRE(box.value().bottom == 10u);
        }
    }

    SECTION("shrinkTo")
    {
        SECTION("correctly downsizes map into selected rect")
        {
            LevelD level = LeveldBuilder::buildWithMesh(
                20, 20, { { 5, 5, 1, 1 }, { 10, 10, 2, 0 } });

            mesh.loadFrom(level);

            auto box = TileRect(5u, 5u, 10u, 10u);
            const unsigned width = 10u - 5u + 1u;
            const unsigned height = 10u - 5u + 1u;

            mesh.shrinkTo(box);

            LevelD exported;
            mesh.saveTo(exported);

            auto&& mesh = exported.mesh;
            REQUIRE_FALSE(mesh.layers.empty());

            auto&& layer = mesh.layers[0];
            REQUIRE(layer.tiles.size() == width * height);
            REQUIRE(layer.tiles.front() == 1u);
            REQUIRE(layer.blocks.front() == true);

            REQUIRE(layer.tiles.back() == 2u);
            REQUIRE(layer.blocks.back() == false);
        }
    }

    SECTION("penCancel cancels drag draw")
    {
        LevelD level = LeveldBuilder::buildWithMesh(
            20, 20, { { 5, 5, 1, 1 }, { 10, 10, 2, 0 } });

        mesh.loadFrom(level);
        mesh.changeDrawingMode(ToolMesh::DrawMode::RectEdge);
        mesh.penDragStarted({ 0, 0 });
        mesh.penDragCancel({ 0, 0 });
        mesh.penDragEnded({ 0, 0 }, { 0, 0 });
        REQUIRE(commandQueue->isEmpty());
    }
}

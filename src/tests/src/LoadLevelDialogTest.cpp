#include "TestHelpers/TestAssets.hpp"
#include <Dialogs/LoadLevelDialog.hpp>
#include <catch.hpp>
#include <fstream>

namespace fs = std::filesystem;

void mockFilesystem(const fs::path& rootDir, unsigned mappacks)
{
    auto&& levelsDir = rootDir / "resources" / "levels";
    fs::create_directories(levelsDir);

    for (unsigned i = 0; i < mappacks; ++i)
    {
        auto&& packDir = levelsDir / ("pack" + std::to_string(i));
        fs::create_directory(packDir);
        auto&& save = std::ofstream(packDir / "map.lvd");
        save << "data";
    }
}

void clearFilesystem(const fs::path& rootDir)
{
    fs::remove_all(rootDir / "resources");
}

TEST_CASE("[LoadLevelDialog]")
{
    mockFilesystem(BINARY_DIR, 1);

    auto&& gui = mem::Rc<Gui>();
    auto&& dialog = LoadLevelDialog(gui, BINARY_DIR / "resources");

    dialog.open([] {});
    {
        auto&& dropdown = gui->get<tgui::ComboBox>("SelectPackId");
        auto&& items = dropdown->getItems();
        REQUIRE(items.size() == 1u);
        REQUIRE(items[0] == "pack0");
    }
    dialog.close();

    // Update file structure
    clearFilesystem(BINARY_DIR);
    mockFilesystem(BINARY_DIR, 2);

    // Reopen dialog, verify dialog noticed it
    dialog.open([] {});
    {
        auto&& dropdown = gui->get<tgui::ComboBox>("SelectPackId");
        auto&& items = dropdown->getItems();
        REQUIRE(items.size() == 2u);
        REQUIRE(items[0] == "pack0");
        REQUIRE(items[1] == "pack1");
    }
    dialog.close();

    clearFilesystem(BINARY_DIR);
}
#include <Dialogs/NewLevelDialog.hpp>
#include <catch.hpp>

TEST_CASE("[NewLevelDialog]")
{
    auto&& gui = mem::Rc<Gui>();
    auto dialog = ModernNewLevelDialog(gui);

    SECTION(
        "Reopening dialog will initialize map compat dropdown to previously "
        "used value")
    {
        dialog.open([] {});

        {
            auto dropdown = gui->get<tgui::ComboBox>("SelectMapCompat");
            dropdown->setSelectedItemByIndex(
                std::to_underlying(MapCompatibility::SingleFlagCTF));
        }

        dialog.close();

        dialog.open([] {});

        {
            auto dropdown = gui->get<tgui::ComboBox>("SelectMapCompat");
            REQUIRE(
                dropdown->getSelectedItemIndex()
                == std::to_underlying(MapCompatibility::SingleFlagCTF));
        }
    }
}
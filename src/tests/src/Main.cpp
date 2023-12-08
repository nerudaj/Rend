#define CATCH_CONFIG_RUNNER
#include <DGM/dgm.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <catch.hpp>

int main(int argc, char* argv[])
{
    // This code just initializes backend for TGUI so we can
    // call tgui-backend-specific code in random tests
    auto window = dgm::Window({ 1, 1 }, "", false);
    tgui::Gui _gui;
    _gui.setWindow(window.getWindowContext());

    int result = Catch::Session().run(argc, argv);

    window.close();

    return result;
}

#include "app/AppStateMenuOptions.hpp"
#include "Utilities/TguiHelper.hpp"
#include <Configs/Sizers.hpp>
#include <Configs/Strings.hpp>
#include <format>
#include <ranges>

import WidgetBuilder;
import LayoutBuilder;

static const std::vector<std::string> STRING_RESOLUTIONS = {
    "640x480",  "800x600",   "1024x768",  "1280x720", "1366x768",
    "1600x900", "1920x1080", "2560x1440", "2736x1824"
};

static const std::vector<sf::Vector2u> NUM_RESOLUTIONS = {
    { 640, 480 },  { 800, 600 },   { 1024, 768 },  { 1280, 720 }, { 1366, 768 },
    { 1600, 900 }, { 1920, 1080 }, { 2560, 1440 }, { 2736, 1824 }
};

std::string getWindowResolutionAsString(const dgm::Window& window)
{
    auto size = window.getSize();
    return std::to_string(size.x) + "x" + std::to_string(size.y);
}

void AppStateMenuOptions::buildLayoutImpl()
{
    auto&& basePanel = WidgetBuilder::createPanel();

    auto&& panel = WidgetBuilder::createPanel(
        { "100%",
          ("100% - " + std::to_string(Sizers::getBaseContainerHeight()))
              .c_str() });
    panel->setPosition({ "0%", Sizers::getBaseContainerHeight() });

    basePanel->add(panel, "IdTabPanel");

    auto&& tabs = tgui::Tabs::create();
    tabs->setSize({ "100%", Sizers::getBaseContainerHeight() });
    tabs->setTextSize(Sizers::getBaseTextSize());
    tabs->add(Strings::AppState::Options::DISPLAY);
    tabs->add(Strings::AppState::Options::AUDIO);
    tabs->add(Strings::AppState::Options::INPUT);
    tabs->onTabSelect(
        [&](const tgui::String& tabname)
        {
            auto panel = gui->get<tgui::Panel>("IdTabPanel");
            panel->removeAllWidgets();

            auto&& builder = FormBuilder();
            if (tabname == Strings::AppState::Options::DISPLAY)
            {
                buildDisplayOptionsLayout(builder);
            }
            else if (tabname == Strings::AppState::Options::AUDIO)
            {
                buildAudioOptionsLayout(builder);
            }
            else if (tabname == Strings::AppState::Options::INPUT)
            {
                buildInputOptionsLayout(builder);
            }
            panel->add(builder.build(PANEL_BACKGROUND_COLOR));
        });
    basePanel->add(tabs);

    gui->add(
        LayoutBuilder::withBackgroundImage(
            resmgr->get<sf::Texture>("menu_options.png").value().get())
            .withTitle(Strings::AppState::Options::TITLE, HeadingLevel::H1)
            .withContent(basePanel)
            .withBackButton(WidgetBuilder::createButton(
                Strings::AppState::MainMenu::BACK, [this] { app.popState(); }))
            .withNoSubmitButton()
            .build());
    tabs->select(Strings::AppState::Options::DISPLAY);
}

void AppStateMenuOptions::buildDisplayOptionsLayout(FormBuilder& builder)
{
    auto fovFormatter = [](float fov)
    { return std::to_string(static_cast<int>(fov * 100)); };

    std::ignore =
        builder
            .addOption(
                Strings::AppState::Options::SET_RESOLUTION,
                WidgetBuilder::createDropdown(
                    STRING_RESOLUTIONS,
                    getWindowResolutionAsString(app.window),
                    [this](std::size_t idx)
                    {
                        if (idx == -1) return;

                        // Restart window with new resolution
                        app.window.changeResolution(NUM_RESOLUTIONS[idx]);

                        // Force gui to update viewport and resolution
                        restoreFocusImpl();
                    }))
            .addOption(
                Strings::AppState::Options::FULLSCREEN,
                WidgetBuilder::createCheckbox(
                    app.window.isFullscreen(),
                    [this](bool) { app.window.toggleFullscreen(); }))
            .addOption(
                Strings::AppState::Options::USE_DITHERED_SHADES,
                WidgetBuilder::createCheckbox(
                    settings->display.useDitheredShadows,
                    [this](bool value)
                    { settings->display.useDitheredShadows = value; }))
            .addOption(
                Strings::AppState::Options::SHOW_CROSSHAIR,
                WidgetBuilder::createCheckbox(
                    settings->display.showCrosshair,
                    [this](bool value)
                    { settings->display.showCrosshair = value; }))
            .addOption(
                Strings::AppState::Options::FOV,
                WidgetBuilder::createSlider(
                    settings->display.fov,
                    [this](float value) { settings->display.fov = value; },
                    gui,
                    fovFormatter,
                    0.6f,
                    1.2f,
                    0.01f));
}

void AppStateMenuOptions::buildAudioOptionsLayout(FormBuilder& builder)
{
    auto volumeFormatter = [](float vol)
    { return std::to_string(static_cast<int>(vol)); };

    std::ignore =
        builder
            .addOption(
                Strings::AppState::Options::SOUND_VOLUME,
                WidgetBuilder::createSlider(
                    settings->audio.soundVolume,
                    [this](float value)
                    {
                        settings->audio.soundVolume = value;
                        audioPlayer->setSoundVolume(
                            settings->audio.soundVolume);
                    },
                    gui,
                    volumeFormatter))
            .addOption(
                Strings::AppState::Options::MUSIC_VOLUME,
                WidgetBuilder::createSlider(
                    settings->audio.musicVolume,
                    [this](float value)
                    {
                        settings->audio.musicVolume = value;
                        jukebox->setVolume(settings->audio.musicVolume);
                    },
                    gui,
                    volumeFormatter));
}

void AppStateMenuOptions::buildInputOptionsLayout(FormBuilder& builder)
{
    auto sensitivityFormatter = [](float val)
    { return std::to_string(static_cast<int>(val)); };
    auto turnSpeedFormatter = [](float val)
    { return std::format("{:.1f}", val); };
    auto deadzoneFormatter = [](float val)
    { return std::format("{:.2f}", val); };

    std::ignore =
        builder
            .addOption(
                Strings::AppState::Options::MOUSE_SENSITIVITY,
                WidgetBuilder::createSlider(
                    settings->input.mouseSensitivity,
                    [this](float value)
                    { settings->input.mouseSensitivity = value; },
                    gui,
                    sensitivityFormatter,
                    1.f,
                    50.f,
                    1.f))
            .addOption(
                Strings::AppState::Options::TURN_SENSITIVITY,
                WidgetBuilder::createSlider(
                    settings->input.turnSpeed,
                    [this](float value) { settings->input.turnSpeed = value; },
                    gui,
                    turnSpeedFormatter,
                    0.1f,
                    5.f,
                    0.1f))
            .addOption(
                Strings::AppState::Options::GAMEPAD_DEADZONE,
                WidgetBuilder::createSlider(
                    settings->input.gamepadDeadzone,
                    [this](float value)
                    { settings->input.gamepadDeadzone = value; },
                    gui,
                    deadzoneFormatter,
                    0.f,
                    1.f,
                    0.01f))
            .addOption(
                Strings::AppState::Options::SWAP_ON_PICKUP,
                WidgetBuilder::createCheckbox(
                    settings->input.autoswapOnPickup,
                    [this](bool value)
                    { settings->input.autoswapOnPickup = value; }),
                enteredFromGame);
}

void AppStateMenuOptions::input()
{
    sf::Event event;
    while (app.window.pollEvent(event))
    {
        gui->handleEvent(event);
    }

    controller->update();
}

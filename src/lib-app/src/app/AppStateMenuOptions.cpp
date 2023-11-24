#include "app/AppStateMenuOptions.hpp"
#include "app/GuiBuilder.hpp"
#include <Configs/Strings.hpp>
#include <ranges>

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
    gui->add(createH2Title(Strings::AppState::Options::TITLE));

    auto panel = tgui::Panel::create();
    panel->setPosition("20%", "35%");
    panel->setSize("60%", "50%");
    gui->add(panel, "IdTabPanel");

    auto tabs = tgui::Tabs::create();
    tabs->setPosition("20%", "30%");
    tabs->setSize("60%", "5%");
    tabs->add(Strings::AppState::Options::DISPLAY);
    tabs->add(Strings::AppState::Options::AUDIO);
    tabs->add(Strings::AppState::Options::INPUT);
    tabs->onTabSelect(
        [&](const tgui::String& tabname)
        {
            auto panel = gui->get<tgui::Panel>("IdTabPanel");
            panel->removeAllWidgets();

            if (tabname == Strings::AppState::Options::DISPLAY)
            {
                auto&& builder = GuiOptionsBuilder2(panel);
                buildDisplayOptionsLayout(builder);
                builder.build();
            }
            else if (tabname == Strings::AppState::Options::AUDIO)
            {
                auto&& builder = GuiOptionsBuilder2(panel);
                buildAudioOptionsLayout(builder);
                builder.build();
            }
            else if (tabname == Strings::AppState::Options::INPUT)
            {
                auto&& builder = GuiOptionsBuilder2(panel);
                buildInputOptionsLayout(builder);
                builder.build();
            }
        });
    tabs->select(Strings::AppState::Options::DISPLAY);
    gui->add(tabs);

    gui->add(createBackButton([this] { app.popState(); }));
}

void AppStateMenuOptions::buildDisplayOptionsLayout(GuiOptionsBuilder2& builder)
{
    builder
        .addOption(
            Strings::AppState::Options::FULLSCREEN,
            WidgetCreator2::createCheckbox(
                app.window.isFullscreen(),
                [this](bool) { app.window.toggleFullscreen(); }))
        .addOption(
            Strings::AppState::Options::SET_RESOLUTION,
            WidgetCreator2::createDropdown(
                STRING_RESOLUTIONS,
                getWindowResolutionAsString(app.window),
                [this](std::size_t idx)
                {
                    if (idx == -1) return;

                    // Restart window with new resolution
                    app.window.changeResolution(NUM_RESOLUTIONS[idx]);

                    // Force gui to update viewport and resolution
                    restoreFocus();
                }));
}

void AppStateMenuOptions::buildAudioOptionsLayout(GuiOptionsBuilder2& builder)
{
    builder
        .addOption(
            Strings::AppState::Options::SOUND_VOLUME,
            WidgetCreator2::createSlider(
                settings->audio.soundVolume,
                [this](float value)
                {
                    settings->audio.soundVolume = value;
                    audioPlayer->setSoundVolume(settings->audio.soundVolume);
                }))
        .addOption(
            Strings::AppState::Options::MUSIC_VOLUME,
            WidgetCreator2::createSlider(
                settings->audio.musicVolume,
                [this](float value)
                {
                    settings->audio.musicVolume = value;
                    audioPlayer->setSoundVolume(settings->audio.musicVolume);
                }));
}

void AppStateMenuOptions::buildInputOptionsLayout(GuiOptionsBuilder2& builder)
{
    builder.addOption(
        Strings::AppState::Options::MOUSE_SENSITIVITY,
        WidgetCreator2::createSlider(
            settings->input.mouseSensitivity,
            [this](float value) { settings->input.mouseSensitivity = value; }));
}

void AppStateMenuOptions::input()
{
    sf::Event event;
    while (app.window.pollEvent(event))
    {
        gui->handleEvent(event);
    }
}

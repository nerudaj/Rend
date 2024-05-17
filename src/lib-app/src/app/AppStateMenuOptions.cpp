#include "app/AppStateMenuOptions.hpp"
#include "GuiBuilder.hpp"
#include "Utilities/TguiHelper.hpp"
#include "utils/InputHandler.hpp"
#include <Configs/Sizers.hpp>
#include <Configs/Strings.hpp>
#include <format>
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

void AppStateMenuOptions::buildLayout()
{
    auto&& tabs = WidgetBuilder::createTabbedContent(
        { Strings::AppState::Options::DISPLAY,
          Strings::AppState::Options::AUDIO,
          Strings::AppState::Options::INPUT,
          Strings::AppState::Options::PLAYER },
        "IdOptionsTabPanel",
        [&](const tgui::String& tabname) { handleTabSelected(tabname); });

    dic->gui->rebuildWith(
        LayoutBuilder::withBackgroundImage(
            dic->resmgr->get<sf::Texture>("menu_options.png").value().get())
            .withTitle(Strings::AppState::Options::TITLE, HeadingLevel::H1)
            .withContent(tabs)
            .withBackButton(WidgetBuilder::createButton(
                Strings::AppState::MainMenu::BACK, [this] { app.popState(); }))
            .withNoSubmitButton()
            .build());

    handleTabSelected(Strings::AppState::Options::DISPLAY);
}

void AppStateMenuOptions::buildDisplayOptionsLayout(FormBuilder& builder)
{
    auto fovFormatter = [](float fov)
    { return std::to_string(static_cast<int>(fov * 100)); };

    auto hudScaleFormatter = [](float scale)
    { return std::format("{:.1f}", scale); };

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
                        dic->gui->gui.setWindow(app.window.getWindowContext());

                        warningDialog.open(Strings::AppState::Options::
                                               RESOLUTION_CHANGE_WARNING);
                    }))
            .addOption(
                Strings::AppState::Options::FULLSCREEN,
                WidgetBuilder::createCheckbox(
                    app.window.isFullscreen(),
                    [this](bool) { app.window.toggleFullscreen(); }))
            .addOption(
                Strings::AppState::Options::USE_DITHERED_SHADES,
                WidgetBuilder::createCheckbox(
                    dic->settings->display.useDitheredShadows,
                    [this](bool value)
                    { dic->settings->display.useDitheredShadows = value; }))
            .addOption(
                Strings::AppState::Options::SHOW_CROSSHAIR,
                WidgetBuilder::createCheckbox(
                    dic->settings->display.showCrosshair,
                    [this](bool value)
                    { dic->settings->display.showCrosshair = value; }))
            .addOption(
                Strings::AppState::Options::FOV,
                WidgetBuilder::createSlider(
                    dic->settings->display.fov,
                    [this](float value) { dic->settings->display.fov = value; },
                    dic->gui->gui,
                    fovFormatter,
                    0.6f,
                    1.2f,
                    0.01f))
            .addOption(
                Strings::AppState::Options::HUD_UI_SCALE,
                WidgetBuilder::createSlider(
                    dic->settings->display.hudScale,
                    [this](float value)
                    { dic->settings->display.hudScale = value; },
                    dic->gui->gui,
                    hudScaleFormatter,
                    1.f,
                    4.f,
                    0.5f));
}

void AppStateMenuOptions::buildAudioOptionsLayout(FormBuilder& builder)
{
    auto volumeFormatter = [](float vol)
    { return std::to_string(static_cast<int>(vol)); };

    std::ignore = builder
                      .addOption(
                          Strings::AppState::Options::SOUND_VOLUME,
                          WidgetBuilder::createSlider(
                              dic->settings->audio.soundVolume,
                              [this](float value)
                              {
                                  dic->settings->audio.soundVolume = value;
                                  dic->audioPlayer->setSoundVolume(
                                      dic->settings->audio.soundVolume);
                              },
                              dic->gui->gui,
                              volumeFormatter))
                      .addOption(
                          Strings::AppState::Options::MUSIC_VOLUME,
                          WidgetBuilder::createSlider(
                              dic->settings->audio.musicVolume,
                              [this](float value)
                              {
                                  dic->settings->audio.musicVolume = value;
                                  dic->jukebox->setVolume(
                                      dic->settings->audio.musicVolume);
                              },
                              dic->gui->gui,
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

    std::ignore = builder
                      .addOption(
                          Strings::AppState::Options::MOUSE_SENSITIVITY,
                          WidgetBuilder::createSlider(
                              dic->settings->input.mouseSensitivity,
                              [this](float value) {
                                  dic->settings->input.mouseSensitivity = value;
                              },
                              dic->gui->gui,
                              sensitivityFormatter,
                              1.f,
                              50.f,
                              1.f))
                      .addOption(
                          Strings::AppState::Options::TURN_SENSITIVITY,
                          WidgetBuilder::createSlider(
                              dic->settings->input.turnSpeed,
                              [this](float value)
                              { dic->settings->input.turnSpeed = value; },
                              dic->gui->gui,
                              turnSpeedFormatter,
                              0.1f,
                              5.f,
                              0.1f))
                      .addOption(
                          Strings::AppState::Options::GAMEPAD_DEADZONE,
                          WidgetBuilder::createSlider(
                              dic->settings->input.gamepadDeadzone,
                              [this](float value)
                              { dic->settings->input.gamepadDeadzone = value; },
                              dic->gui->gui,
                              deadzoneFormatter,
                              0.f,
                              1.f,
                              0.01f));
}

void AppStateMenuOptions::buildPlayerOptionsLayout(
    FormBuilder& builder,
    mem::Rc<DependencyContainer>& dic,
    bool changesDisabled,
    std::function<void(void)> onChanged)
{
    std::ignore = builder
                      .addOption(
                          Strings::AppState::Options::PLAYER_NAME,
                          WidgetBuilder::createTextInput(
                              dic->settings->player.name,
                              [&, onChanged](const tgui::String& text)
                              {
                                  dic->settings->player.name =
                                      text.toStdString();
                                  onChanged();
                              }),
                          changesDisabled)
                      .addOption(
                          Strings::AppState::Options::SWAP_ON_PICKUP,
                          WidgetBuilder::createCheckbox(
                              dic->settings->input.autoswapOnPickup,
                              [&, onChanged](bool value)
                              {
                                  dic->settings->input.autoswapOnPickup = value;
                                  onChanged();
                              }),
                          changesDisabled);
}

void AppStateMenuOptions::handleTabSelected(const tgui::String& selectedTabName)
{
    auto panel = dic->gui->get<tgui::Panel>("IdOptionsTabPanel");
    panel->removeAllWidgets();

    auto&& builder = FormBuilder();
    if (selectedTabName == Strings::AppState::Options::DISPLAY)
    {
        buildDisplayOptionsLayout(builder);
    }
    else if (selectedTabName == Strings::AppState::Options::AUDIO)
    {
        buildAudioOptionsLayout(builder);
    }
    else if (selectedTabName == Strings::AppState::Options::INPUT)
    {
        buildInputOptionsLayout(builder);
    }
    else if (selectedTabName == Strings::AppState::Options::PLAYER)
    {
        buildPlayerOptionsLayout(builder, dic, enteredFromGame);
    }
    panel->add(builder.build(PANEL_BACKGROUND_COLOR));
}

void AppStateMenuOptions::input()
{
    InputHandler::handleUiStateInput(app, *dic);
}

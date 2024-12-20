#include "GuiBuilder.hpp"
#include "LobbySettings.hpp"
#include "utils/AppMessage.hpp"
#include "utils/InputHandler.hpp"
#include <Configs/Strings.hpp>
#include <Filesystem.hpp>
#include <LevelD.hpp>
#include <app/AppStateGameSetup.hpp>
#include <app/AppStateMapRotationWrapper.hpp>
#include <atomic>
#include <enums/MapCompatibility.hpp>
#include <expected>
#include <random>

AppStateGameSetup::AppStateGameSetup(
    dgm::App& app, mem::Rc<DependencyContainer> dic) noexcept
    : AppStateLobbyBase(
          app,
          dic,
          mem::Rc<Client>("127.0.0.1", 10666ui16),
          LobbyBaseConfig { .isHost = true })
    , mapPackNames(Filesystem::getLevelPackNames(
          Filesystem::getLevelsDir(dic->settings->cmdSettings.resourcesDir)))
    , mapPickerDialog(dic->gui, std::vector<MapSettingsForPicker>())
{
    selectMapPack(
        dic->settings->lastUsedLobbySettings.packname.empty()
            ? mapPackNames.front()
            : dic->settings->lastUsedLobbySettings.packname,
        dic->settings->lastUsedLobbySettings.mapSettings);
    sendLobbyUpdate();
}

AppStateGameSetup::~AppStateGameSetup()
{
    dic->settings->lastUsedLobbySettings = lobbySettings;
}

void AppStateGameSetup::input()
{
    if (dic->settings->cmdSettings.skipMainMenu)
    {
        dic->settings->cmdSettings.skipMainMenu = false;
        InputHandler::handleSendReady(app, *dic, client);
    }

    InputHandler::handleUiStateInput(app, *dic);

    dic->logger->ifError(
        0,
        "readIncomingPackets",
        client->readIncomingPackets(std::bind(
            &AppStateGameSetup::handleNetworkUpdate,
            this,
            std::placeholders::_1)));
}

void AppStateGameSetup::buildLayoutGameSetupImpl(tgui::Panel::Ptr target)
{
    auto gameModeNames = std::vector<std::string> {
        Strings::AppState::GameSetup::GAMEMODE_DM,
        Strings::AppState::GameSetup::GAMEMODE_SCTF
    };
    assert(std::to_underlying(lobbySettings.gameMode) < gameModeNames.size());

    target->add(
        FormBuilder()
            .addOption(
                Strings::AppState::GameSetup::SERVER_IP,
                WidgetBuilder::createTextLabel(
                    sf::IpAddress::getLocalAddress().toString()))
            .addOption(
                Strings::AppState::GameSetup::MAX_NPCS,
                WidgetBuilder::createDropdown(
                    { "0", "1", "2", "3" },
                    std::to_string(lobbySettings.maxNpcs),
                    [this](std::size_t idx)
                    {
                        lobbySettings.maxNpcs = idx;
                        sendLobbyUpdate();
                    }))
            .addSeparator()
            .addOption(
                Strings::AppState::GameSetup::SELECT_GAMEMODE,
                WidgetBuilder::createDropdown(
                    gameModeNames,
                    gameModeNames.at(
                        std::to_underlying(lobbySettings.gameMode)),
                    [&](size_t optionIdx)
                    {
                        lobbySettings.gameMode =
                            static_cast<GameMode>(optionIdx);
                        sendLobbyUpdate();
                    }))
            .addOption(
                Strings::AppState::GameSetup::POINTLIMIT,
                WidgetBuilder::createTextInput(
                    std::to_string(lobbySettings.pointlimit),
                    [this](const tgui::String& newValue)
                    {
                        if (newValue.empty()) return;
                        lobbySettings.pointlimit =
                            std::stoi(std::string(newValue));
                        sendLobbyUpdate();
                    },
                    WidgetBuilder::getPositiveNumericValidator()))
            .addSeparator()
            .addOption(
                Strings::AppState::GameSetup::SELECT_PACK,
                WidgetBuilder::createDropdown(
                    mapPackNames,
                    lobbySettings.packname,
                    [this](std::size_t idx)
                    { selectMapPackAndSendUpdate(mapPackNames.at(idx)); }))
            .addOption(
                Strings::AppState::GameSetup::RANDOM_ROTATION,
                WidgetBuilder::createCheckbox(
                    lobbySettings.useRandomMapRotation,
                    [&](bool value)
                    {
                        lobbySettings.useRandomMapRotation = value;
                        adjustMapOrderAndSendUpdate();
                    }))
            .addOption(
                Strings::AppState::GameSetup::SELECT_MAPS,
                WidgetBuilder::createButton(
                    Strings::AppState::GameSetup::DOTDOTDOT,
                    std::bind(&AppStateGameSetup::openMapPicker, this)))
            .build(PANEL_BACKGROUND_COLOR));
}

void AppStateGameSetup::restoreFocusImpl(const std::string& message)
{
    buildLayout();
    handleAppMessage<AppStateGameSetup>(app, message);
}

void AppStateGameSetup::selectMapPack(
    const std::string& packname, const std::vector<MapOptions>& mapOptionsHint)
{
    lobbySettings.packname = packname;

    const auto levelNames = Filesystem::getLevelNames(
        Filesystem::getLevelsDir(dic->settings->cmdSettings.resourcesDir),
        packname);

    const bool allMapsMatchWithHint =
        mapOptionsHint.size() == levelNames.size()
        && std::ranges::all_of(
            std::views::zip(levelNames, mapOptionsHint),
            [](const std::tuple<std::string, MapOptions>& t)
            { return std::get<0>(t) == std::get<1>(t).name; });

    lobbySettings.mapSettings =
        allMapsMatchWithHint
            ? mapOptionsHint
            : levelNames
                  | std::views::transform(
                      [](const std::string& name)
                      { return MapOptions { .name = name, .enabled = true }; })
                  | std::ranges::to<std::vector>();

    lobbySettings.mapOrder =
        std::views::iota(size_t { 0 }, lobbySettings.mapSettings.size())
        | std::ranges::to<std::vector>();
}

void AppStateGameSetup::selectMapPackAndSendUpdate(const std::string& packname)
{
    selectMapPack(packname);

    if (lobbySettings.useRandomMapRotation) shuffleMapOrder();

    sendLobbyUpdate();
}

void AppStateGameSetup::adjustMapOrderAndSendUpdate()
{
    if (lobbySettings.useRandomMapRotation)
        shuffleMapOrder();
    else
        std::ranges::sort(lobbySettings.mapOrder);

    sendLobbyUpdate();
}

void AppStateGameSetup::shuffleMapOrder()
{
    auto&& generator = std::mt19937(std::random_device()());
    std::shuffle(
        lobbySettings.mapOrder.begin(),
        lobbySettings.mapOrder.end(),
        generator);
}

void AppStateGameSetup::openMapPicker()
{
    mapPickerDialog = mem::Box<MapPickerDialog>(
        dic->gui,
        lobbySettings.mapSettings
            | std::views::transform(
                [](auto s)
                { return MapSettingsForPicker { s.name, s.enabled }; })
            | std::ranges::to<std::vector>());
    mapPickerDialog->open(
        std::bind(&AppStateGameSetup::handleMapRotationUpdate, this));
}

void AppStateGameSetup::handleMapRotationUpdate()
{
    lobbySettings.mapSettings =
        mapPickerDialog->getMapSettings()
        | std::views::transform([](auto s)
                                { return MapOptions { s.name, s.enabled }; })
        | std::ranges::to<std::vector>();
    sendLobbyUpdate();
}

void AppStateGameSetup::sendLobbyUpdate()
{
    dic->logger->ifError(
        0,
        "sendLobbySettingsUpdate",
        client->sendLobbySettingsUpdate(lobbySettings));
}

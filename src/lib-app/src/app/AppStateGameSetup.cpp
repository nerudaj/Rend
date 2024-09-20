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
#include <expected>

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
    selectMapPack(mapPackNames.front());
    sendLobbyUpdate();
}

void AppStateGameSetup::input()
{
    if (dic->settings->cmdSettings.skipMainMenu)
    {
        dic->settings->cmdSettings.skipMainMenu = false;
        InputHandler::handleSendReady(app, *dic, client);
    }

    InputHandler::handleUiStateInput(app, *dic);

    dic->logger->log(client->readIncomingPackets(std::bind(
        &AppStateGameSetup::handleNetworkUpdate, this, std::placeholders::_1)));
}

void AppStateGameSetup::buildLayoutGameSetupImpl(tgui::Panel::Ptr target)
{
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
            .addOption(
                Strings::AppState::GameSetup::SELECT_PACK,
                WidgetBuilder::createDropdown(
                    mapPackNames,
                    lobbySettings.packname,
                    [this](std::size_t idx)
                    { selectMapPackAndSendUpdate(mapPackNames.at(idx)); }))
            .addOption(
                Strings::AppState::GameSetup::SELECT_MAPS,
                WidgetBuilder::createButton(
                    Strings::AppState::GameSetup::DOTDOTDOT,
                    std::bind(&AppStateGameSetup::openMapPicker, this)))
            .addOption(
                Strings::AppState::GameSetup::FRAGLIMIT,
                WidgetBuilder::createTextInput(
                    std::to_string(lobbySettings.fraglimit),
                    [this](const tgui::String& newValue)
                    {
                        if (newValue.empty()) return;
                        lobbySettings.fraglimit =
                            std::stoi(std::string(newValue));
                        sendLobbyUpdate();
                    },
                    WidgetBuilder::getPositiveNumericValidator()))
            .build(PANEL_BACKGROUND_COLOR));
}

void AppStateGameSetup::restoreFocusImpl(const std::string& message)
{
    buildLayout();
    handleAppMessage<AppStateGameSetup>(app, message);
}

void AppStateGameSetup::selectMapPack(const std::string& packname)
{
    lobbySettings.packname = packname;
    lobbySettings.mapSettings =
        Filesystem::getLevelNames(
            Filesystem::getLevelsDir(dic->settings->cmdSettings.resourcesDir),
            packname)
        | std::views::transform(
            [](const std::string& name)
            { return MapSettings { .name = name, .enabled = true }; })
        | std::ranges::to<std::vector>();
    lobbySettings.mapOrder =
        std::views::iota(size_t { 0 }, lobbySettings.mapSettings.size())
        | std::ranges::to<std::vector>();
}

void AppStateGameSetup::selectMapPackAndSendUpdate(const std::string& packname)
{
    selectMapPack(packname);
    sendLobbyUpdate();
}

void AppStateGameSetup::openMapPicker()
{
    mapPickerDialog = mem::Box<MapPickerDialog>(
        dic->gui,
        lobbySettings.mapSettings
            | std::views::transform(
                [](auto s) {
                    return MapSettingsForPicker { s.name, s.enabled };
                })
            | std::ranges::to<std::vector>());
    mapPickerDialog->open(
        std::bind(&AppStateGameSetup::handleMapRotationUpdate, this));
}

void AppStateGameSetup::handleMapRotationUpdate()
{
    lobbySettings.mapSettings =
        mapPickerDialog->getMapSettings()
        | std::views::transform(
            [](auto s) {
                return MapSettings { s.name, s.enabled };
            })
        | std::ranges::to<std::vector>();
    sendLobbyUpdate();
}

void AppStateGameSetup::sendLobbyUpdate()
{
    dic->logger->log(client->sendLobbySettingsUpdate(lobbySettings));
}

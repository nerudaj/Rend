#include "app/AppStatePeerLobby.hpp"
#include "utils/AppMessage.hpp"
#include "utils/InputHandler.hpp"
#include <Configs/Strings.hpp>
#include <CoreTypes.hpp>
#include <GuiBuilder.hpp>

AppStatePeerLobby::AppStatePeerLobby(
    dgm::App& app,
    mem::Rc<DependencyContainer> dic,
    const sf::IpAddress& hostAddress)
    : AppStateLobbyBase(app, dic, mem::Rc<Client>(hostAddress, 10666))
{
}

void AppStatePeerLobby::input()
{
    InputHandler::handleUiStateInput(app, *dic);

    dic->logger->ifError(
        0,
        "readIncomingPackets",
        client->readIncomingPackets(
            std::bind(
                &AppStatePeerLobby::handleNetworkUpdate,
                this,
                std::placeholders::_1),
            std::bind(
                &AppStatePeerLobby::handleMapDownload,
                this,
                std::placeholders::_1)));
}

void AppStatePeerLobby::buildLayoutGameSetupImpl(tgui::Panel::Ptr target) {}

void AppStatePeerLobby::restoreFocusImpl(const std::string& message)
{
    buildLayout();
    handleAppMessage<AppStatePeerLobby>(app, message);
}

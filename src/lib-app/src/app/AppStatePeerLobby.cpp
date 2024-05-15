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
    : dgm::AppState(app)
    , dic(dic)
    , client(mem::Rc<Client>(hostAddress, 10666))
    , myPeerData(ClientData { .active = true,
                              .name = "player",
                              .isReady = false,
                              .hasAutoswapOnPickup =
                                  dic->settings->input.autoswapOnPickup })
{
    buildLayout();
    client->sendPeerUpdate(myPeerData);
}

void AppStatePeerLobby::input()
{
    InputHandler::handleUiStateInput(app, *dic);

    client->readIncomingPackets(std::bind(
        &AppStatePeerLobby::handleLobbyUpdate, this, std::placeholders::_1));
}

void AppStatePeerLobby::buildLayout()
{
    auto&& table =
        TableBuilder().withHeading({ Strings::AppState::PeerLobby::PNAME,
                                     Strings::AppState::PeerLobby::IS_READY });
    for (auto&& peer : connectedPeers)
        table.addRow({ peer.name,
                       peer.isReady
                           ? Strings::AppState::PeerLobby::READY
                           : Strings::AppState::PeerLobby::NOT_READY });

    dic->gui->rebuildWith(
        LayoutBuilder()
            .withBackgroundImage(
                dic->resmgr->get<sf::Texture>("menu_setup.png").value().get())
            .withTitle(Strings::AppState::PeerLobby::TITLE, HeadingLevel::H1)
            .withContent(table.build())
            .withBackButton(WidgetBuilder::createButton(
                Strings::AppState::MainMenu::BACK, [&] { app.popState(); }))
            .withSubmitButton(WidgetBuilder::createButton(
                Strings::AppState::PeerLobby::READY,
                [&] { client->sendPeerReadySignal(); }))
            .build());
}

void AppStatePeerLobby::restoreFocusImpl(const std::string& message)
{
    buildLayout();
    handleAppMessage<decltype(this)>(app, message);
}

void AppStatePeerLobby::handleLobbyUpdate(const ServerUpdateData& update)
{
    connectedPeers = update.clients;
    lobbySettings = update.lobbySettings;
    buildLayout();

    if (update.lobbyCommited && update.peersReady)
    {
        // TODO: spawn game
    }
}

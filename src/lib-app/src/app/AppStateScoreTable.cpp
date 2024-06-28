#include "GuiBuilder.hpp"
#include "utils/AppMessage.hpp"
#include "utils/ClientStateToString.hpp"
#include "utils/InputHandler.hpp"
#include <Configs/Sizers.hpp>
#include <Configs/Strings.hpp>
#include <app/AppStateScoreTable.hpp>

void AppStateScoreTable::input()
{
    InputHandler::handleUiStateInputWithoutGoBackOption(app, *dic);

    dic->logger->log(client->readIncomingPackets(std::bind(
        &AppStateScoreTable::handleNetworkUpdate,
        this,
        std::placeholders::_1)));
}

void AppStateScoreTable::buildLayout()
{
    struct [[nodiscard]] PlayerScore final
    {
        std::string name;
        int score;
        ClientState state;
    };

    // NOTE: this zipping discards bots, they should be marked as ready, because
    // clientData
    auto orderedScores =
        std::views::zip(scores, gameSettings.players, clientData)
        | std::views::transform(
            [](const std::tuple<int, PlayerOptions, ClientData>& t)
            {
                return PlayerScore { .name = std::get<1>(t).name,
                                     .score = std::get<0>(t),
                                     .state = std::get<2>(t).state };
            })
        | std::ranges::to<std::vector>();

    std::ranges::sort(
        orderedScores,
        [](const PlayerScore& a, const PlayerScore& b) -> bool
        { return a.score >= b.score; });

    auto&& tableBuilder =
        TableBuilder().withHeading({ Strings::AppState::Scores::PLAYER_TH,
                                     Strings::AppState::Scores::SCORE_TH,
                                     Strings::AppState::PeerLobby::IS_READY });
    for (auto&& score : orderedScores)
    {
        tableBuilder.addRow({ score.name,
                              std::to_string(score.score),
                              clientStateToString(score.state) });
    }

    dic->gui->rebuildWith(
        LayoutBuilder::withBackgroundImage(
            dic->resmgr->get<sf::Texture>("menu_intermission.png")
                .value()
                .get())
            .withTitle(Strings::AppState::Scores::TITLE, HeadingLevel::H1)
            .withContent(tableBuilder.build())
            .withBackButton(WidgetBuilder::createButton(
                Strings::AppState::MainMenu::BACK_TO_MENU,
                [this] { app.popState(PopIfNotMainMenu::serialize()); }))
            .withSubmitButton(WidgetBuilder::createButton(
                Strings::AppState::MainMenu::NEXT_MAP,
                [this] { dic->logger->log(client->sendPeerReadySignal()); }))
            .build());
}

void AppStateScoreTable::handleNetworkUpdate(const ServerUpdateData& update)
{
    clientData = update.clients;

    if (update.state == ServerState::MapLoading)
    {
        app.popState(PopIfNotMapRotationWrapper::serialize());
    }
    else
    {
        buildLayout();
    }
}

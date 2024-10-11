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

    dic->logger->ifError(
        0,
        "readIncomingPackets",
        client->readIncomingPackets(std::bind(
            &AppStateScoreTable::handleNetworkUpdate,
            this,
            std::placeholders::_1)));
}

struct [[nodiscard]] PlayerScore final
{
    std::string name;
    int score;
    ptrdiff_t playerIdx;
};

PlayerScore flattenTuplesToPlayerScore(
    const std::tuple<ptrdiff_t, std::tuple<int, PlayerOptions>>& params)
{
    const auto& [idx, t] = params;
    const auto& [score, opts] = t;
    return PlayerScore {
        .name = opts.name,
        .score = score,
        .playerIdx = idx,
    };
}

static std::vector<PlayerScore>
getOrderedScores(const std::vector<int> scores, const GameOptions& gameSettings)
{
    auto&& playerScores =
        std::views::enumerate(std::views::zip(scores, gameSettings.players))
        | std::views::transform(flattenTuplesToPlayerScore)
        | std::ranges::to<std::vector>();

    std::ranges::sort(
        playerScores,
        [](const PlayerScore& a, const PlayerScore& b)
        { return a.score >= b.score; });

    return playerScores;
}

void AppStateScoreTable::buildLayout()
{
    dic->gui->rebuildWith(
        LayoutBuilder::withBackgroundImage(
            dic->resmgr->get<sf::Texture>("menu_intermission.png")
                .value()
                .get())
            .withTitle(Strings::AppState::Scores::TITLE, HeadingLevel::H1)
            .withContent(buildTable())
            .withBackButton(WidgetBuilder::createButton(
                Strings::AppState::MainMenu::BACK_TO_MENU,
                [this] { app.popState(PopIfNotMainMenu::serialize()); }))
            .withSubmitButton(WidgetBuilder::createButton(
                Strings::AppState::MainMenu::NEXT_MAP,
                [&] { InputHandler::handleSendReady(app, *dic, client); }))
            .build());
}

tgui::Panel::Ptr AppStateScoreTable::buildTable() const
{
    auto&& tableBuilder =
        TableBuilder().withHeading({ Strings::AppState::Scores::PLAYER_TH,
                                     Strings::AppState::Scores::SCORE_TH,
                                     Strings::AppState::PeerLobby::IS_READY });

    for (auto&& score : getOrderedScores(scores, gameSettings))
    {
        const auto peerState = score.playerIdx < clientData.size()
                                   ? clientData[score.playerIdx].state
                                   : ClientState::ConnectedAndReady;
        tableBuilder.addRow({ score.name,
                              std::to_string(score.score),
                              clientStateToString(peerState) });
    }

    return tableBuilder.build();
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

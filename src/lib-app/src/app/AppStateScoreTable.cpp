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

void AppStateScoreTable::buildLayout()
{
    struct [[nodiscard]] PlayerScore final
    {
        std::string name;
        int score;
        ClientState state;
    };

    auto&& orderedScores =
        std::views::enumerate(std::views::zip(scores, gameSettings.players))
        | std::views::transform(
            [&](const std::tuple<ptrdiff_t, std::tuple<int, PlayerOptions>>&
                    params)
            {
                auto&& [idx, t] = params;
                return PlayerScore { .name = std::get<1>(t).name,
                                     .score = std::get<0>(t),
                                     .state =
                                         clientData.size() > idx
                                             ? clientData[idx].state
                                             : ClientState::ConnectedAndReady };
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
                [&] { InputHandler::handleSendReady(app, *dic, client); }))
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

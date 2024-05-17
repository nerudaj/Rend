#include "GuiBuilder.hpp"
#include "utils/AppMessage.hpp"
#include "utils/InputHandler.hpp"
#include <Configs/Sizers.hpp>
#include <Configs/Strings.hpp>
#include <app/AppStateScoreTable.hpp>

void AppStateScoreTable::input()
{
    InputHandler::handleUiStateInputWithoutGoBackOption(app, *dic);

    client->readIncomingPackets(std::bind(
        &AppStateScoreTable::handleNetworkUpdate, this, std::placeholders::_1));
}

void AppStateScoreTable::buildLayout()
{
    struct [[nodiscard]] PlayerScore final
    {
        std::string name;
        int score;
    };

    auto orderedScores =
        std::views::zip(scores, gameSettings.players)
        | std::views::transform(
            [](const std::tuple<int, PlayerOptions>& t)
            {
                return PlayerScore { .name = std::get<1>(t).name,
                                     .score = std::get<0>(t) };
            })
        | std::ranges::to<std::vector>();

    std::ranges::sort(
        orderedScores,
        [](const PlayerScore& a, const PlayerScore& b) -> bool
        { return a.score >= b.score; });

    auto&& tableBuilder =
        TableBuilder().withHeading({ Strings::AppState::Scores::PLAYER_TH,
                                     Strings::AppState::Scores::SCORE_TH });
    for (auto&& score : orderedScores)
    {
        tableBuilder.addRow({ score.name, std::to_string(score.score) });
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
                [this] { client->sendPeerReadySignal(); }))
            .build());
}

void AppStateScoreTable::handleNetworkUpdate(const ServerUpdateData& update)
{
    if (update.state == ServerState::MapLoading)
    {
        app.popState(PopIfNotMapRotationWrapper::serialize());
    }
}

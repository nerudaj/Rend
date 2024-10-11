#include "GuiBuilder.hpp"
#include "render/TeamModeScoreHelperForRenderer.hpp"
#include "utils/AppMessage.hpp"
#include "utils/InputHandler.hpp"
#include <Configs/Strings.hpp>
#include <app/AppStateScoreTable.hpp>
#include <app/AppStateWinnerAnnounced.hpp>

void AppStateWinnerAnnounced::input()
{
    InputHandler::handleUiStateInputWithoutGoBackOption(app, *dic);
}

void AppStateWinnerAnnounced::update()
{
    transitionTimeout -= app.time.getDeltaTime();
    if (transitionTimeout <= 0.f)
    {
        app.pushState<AppStateScoreTable>(
            dic,
            client,
            gameSettings,
            scene.playerStates
                | std::views::transform([](const PlayerState& state)
                                        { return state.inventory.score; })
                | std::ranges::to<std::vector<int>>());
    }
}

void AppStateWinnerAnnounced::buildLayout()
{
    auto&& label = WidgetBuilder::createH1Label(
        gameSettings.gameMode == GameMode::Deathmatch ? getSoloWinnerString()
                                                      : getTeamWinnerString());
    label->setSize({ "100%", "25%" });
    label->setPosition({ "0%", "30%" });
    label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    dic->gui->rebuildWith(label);
}

void AppStateWinnerAnnounced::restoreFocusImpl(const std::string& message)
{
    handleAppMessage<AppStateWinnerAnnounced>(app, message);
}

std::string AppStateWinnerAnnounced::getSoloWinnerString() const
{
    return gameSettings.players[getWinnerIndex()].name + " "
           + Strings::AppState::Scores::WON + "!";
}

std::string AppStateWinnerAnnounced::getTeamWinnerString() const
{
    const auto&& teamScores =
        TeamModeScoreHelperForRenderer(scene).getTeamScores();

    return teamScores.redScore > teamScores.blueScore
               ? std::string(Strings::AppState::Scores::RED_TEAM_WON)
               : std::string(Strings::AppState::Scores::BLUE_TEAM_WON);
}

PlayerStateIndexType AppStateWinnerAnnounced::getWinnerIndex() const
{
    const auto&& winnerItr = std::ranges::max_element(
        scene.playerStates,
        [](const PlayerState& a, const PlayerState& b)
        { return a.inventory.score < b.inventory.score; });

    return std::distance(scene.playerStates.begin(), winnerItr);
}

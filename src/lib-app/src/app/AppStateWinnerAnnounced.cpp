#include "GuiBuilder.hpp"
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
        app.pushState<AppStateScoreTable>(dic, client, gameSettings, scores);
    }
}

void AppStateWinnerAnnounced::buildLayout()
{
    auto&& maxIndex = 0u;
    std::array<unsigned, 2> teamScores = { scores[0], 0u };
    for (auto&& i = 1u; i < scores.size(); i++)
    {
        if (scores[i] > scores[maxIndex]) maxIndex = i;
        teamScores[i % 2] += scores[i];
    }

    const auto playerWonStr = gameSettings.players[maxIndex].name + " "
                              + Strings::AppState::Scores::WON + "!";
    // clang-format off
    auto&& label = WidgetBuilder::createH1Label(
        gameSettings.gameMode == GameMode::Deathmatch
        ? playerWonStr
        : teamScores[0] > teamScores[1]
            ? std::string(Strings::AppState::Scores::RED_TEAM_WON)
            : std::string(Strings::AppState::Scores::BLUE_TEAM_WON));
    // clang-format on
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

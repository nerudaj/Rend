#include <Configs/Sizers.hpp>
#include <Configs/Strings.hpp>
#include <app/AppStateScoreTable.hpp>

import WidgetBuilder;
import LayoutBuilder;
import AppMessage;

void AppStateScoreTable::input()
{
    sf::Event event;
    while (app.window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            app.exit();
        }

        gui->handleEvent(event);
    }

    controller->update();
}

[[nodiscard]] static tgui::Label::Ptr
createCell(const std::string& str, unsigned column)
{
    auto label = tgui::Label::create(str);
    label->getRenderer()->setTextColor(tgui::Color::Black);
    label->setSize({ "50%", "100%" });
    label->setPosition({ column == 0 ? "0%" : "50%", "0%" });
    label->setTextSize(Sizers::GetMenuBarTextHeight());
    label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    return label;
}

void AppStateScoreTable::buildLayoutImpl()
{
    struct PlayerScore
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

    auto&& panel = WidgetBuilder::createPanel();

    // Add heading
    {
        auto row = WidgetBuilder::getStandardizedRow(tgui::Color::White);
        row->setPosition({ "0%", "0%" });
        row->add(createCell(Strings::AppState::Scores::PLAYER_TH, 0));
        row->add(createCell(Strings::AppState::Scores::SCORE_TH, 1));
        panel->add(row);
    }

    // Add results
    for (auto&& [idx, score] : std::views::enumerate(orderedScores))
    {
        auto color = idx % 2 == 0 ? tgui::Color(255, 255, 255, 128)
                                  : tgui::Color(255, 255, 255, 192);
        auto row = WidgetBuilder::getStandardizedRow(color);
        row->setPosition({ "0%", row->getSize().y * (idx + 1) });
        row->add(createCell(score.name, 0));
        row->add(createCell(std::to_string(score.score), 1));
        panel->add(row);
    }

    gui->add(
        LayoutBuilder::withBackgroundImage(
            resmgr->get<sf::Texture>("menu_intermission.png").value().get())
            .withTitle(Strings::AppState::Scores::TITLE, HeadingLevel::H1)
            .withContent(panel)
            .withBackButton(WidgetBuilder::createButton(
                Strings::AppState::MainMenu::BACK_TO_MENU,
                [this] { app.popState(PopIfNotMainMenu::serialize()); }))
            .withNoSubmitButton()
            .build());
}

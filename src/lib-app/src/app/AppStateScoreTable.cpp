#include <Configs/Sizers.hpp>
#include <Configs/Strings.hpp>
#include <app/AppStateScoreTable.hpp>

import WidgetBuilder;

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
    gui->add(createBackground(*resmgr, "menu_intermission.png"));
    gui->add(
        createH1Title(Strings::AppState::Scores::TITLE, tgui::Color::White));

    auto panel = createPanel(
        getCommonLayoutPosition(),
        getCommonLayoutSize(scores.size() + 1),
        tgui::Color::Transparent);

    // Add heading
    {
        auto row = WidgetBuilder::getStandardizedRow(tgui::Color::White);
        row->setPosition({ "0%", "0%" });
        row->add(createCell(Strings::AppState::Scores::PLAYER_TH, 0));
        row->add(createCell(Strings::AppState::Scores::SCORE_TH, 1));
        panel->add(row);
    }

    // Add results
    for (auto idx = 0; idx < scores.size(); ++idx)
    {
        auto color = idx % 2 == 0 ? tgui::Color(255, 255, 255, 128)
                                  : tgui::Color(255, 255, 255, 192);
        auto row = WidgetBuilder::getStandardizedRow(color);
        row->setPosition({ "0%", row->getSize().y * (idx + 1) });
        row->add(createCell("player " + std::to_string(idx + 1), 0));
        row->add(createCell(std::to_string(scores[idx]), 1));
        panel->add(row);
    }

    gui->add(panel);

    gui->add(createBackButton([this] { app.popState(3); }, "back to menu"));
}

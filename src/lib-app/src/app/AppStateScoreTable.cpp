#include <app/AppStateScoreTable.hpp>

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
}

void AppStateScoreTable::buildLayoutImpl()
{
    gui->add(createH2Title("results"));

    auto table = tgui::Grid::create();
    table->setSize(getCommonLayoutSize(1u + scores.size()));
    table->setPosition(getCommonLayoutPosition());

    // Add heading
    for (auto column = 0; auto&& labelText : { "player", "score" })
    {
        table->addWidget(tgui::Label::create(labelText), 0, column++);
    }

    // Add results
    for (auto idx = 0; idx < scores.size(); ++idx)
    {
        auto row = idx + 1;
        table->addWidget(
            tgui::Label::create("player " + std::to_string(row)), row, 0);
        table->addWidget(
            tgui::Label::create(std::to_string(scores[idx])), row, 1);
    }

    gui->add(table);

    gui->add(createBackButton([this] { app.popState(3); }, "back to menu"));
}

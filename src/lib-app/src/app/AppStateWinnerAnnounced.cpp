#include <app/AppStateScoreTable.hpp>
#include <app/AppStateWinnerAnnounced.hpp>

import WidgetBuilder;

void AppStateWinnerAnnounced::input()
{
    sf::Event event;
    while (app.window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            app.exit();
        }
    }

    controller->update();
}

void AppStateWinnerAnnounced::update()
{
    transitionTimeout -= app.time.getDeltaTime();
    if (transitionTimeout <= 0.f)
    {
        app.pushState<AppStateScoreTable>(
            resmgr,
            gui,
            audioPlayer,
            jukebox,
            controller,
            gameSettings,
            scores);
    }
}

void AppStateWinnerAnnounced::buildLayoutImpl()
{
    unsigned maxIndex = 0;
    for (unsigned i = 1; i < scores.size(); i++)
    {
        if (scores[i] > scores[maxIndex]) maxIndex = i;
    }

    auto&& label = WidgetBuilder::createH1Label(
        gameSettings.players[maxIndex].name + " won!");
    label->setSize({ "100%", "25%" });
    label->setPosition({ "0%", "30%" });
    label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    gui->add(label);
}

#include <app/AppStateWinnerAnnounced.hpp>

void AppStateWinnerAnnounced::update()
{
    transitionTimeout -= app.time.getDeltaTime();
    if (transitionTimeout <= 0.f) app.popState(3);
}

void AppStateWinnerAnnounced::buildLayoutImpl()
{
    unsigned maxIndex = 0;
    for (unsigned i = 1; i < scores.size(); i++)
    {
        if (scores[i] > scores[maxIndex]) maxIndex = i;
    }
    maxIndex++;

    // TODO: Use player names from settings
    auto label =
        tgui::Label::create("player " + std::to_string(maxIndex) + " won!");
    label->setSize({ "100%", "25%" });
    label->setPosition({ "0%", "30%" });
    label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    label->getRenderer()->setTextColor(tgui::Color::White);
    label->getRenderer()->setTextOutlineColor(tgui::Color::Black);
    label->setTextSize(64);
    gui->add(label);
}

#pragma once

#include "utils/DependencyContainer.hpp"
#include "utils/JoinGameDialog.hpp"
#include <Memory.hpp>
#include <vector>

class [[nodiscard]] AppStateMainMenu final : public dgm::AppState
{
public:
    AppStateMainMenu(dgm::App& app, mem::Rc<DependencyContainer> dic)
        : dgm::AppState(
              app, dgm::AppStateConfig { .clearColor = sf::Color::White })
        , dic(dic)
        , joinGameDialog(
              dic->gui,
              dic->settings->network.joinIpAddress,
              std::bind(
                  &AppStateMainMenu::tryJoinGame, this, std::placeholders::_1))
    {
        buildLayout();
        dic->jukebox->playTitleSong();
        text.setFont(dic->resmgr->get<sf::Font>("pico-8.ttf").value().get());
    }

public:
    void input() override;

    void update() override {}

    void draw() override
    {
        dic->gui->draw();
        dic->virtualCursor->draw(app.window);
        text.setString(dgm::Utility::to_string(
            sf::Mouse::getPosition(app.window.getWindowContext())));
        app.window.draw(text);
    }

private:
    void goToGameSetup();

    void tryJoinGame(const std::string& ip);

    void buildLayout();

    void restoreFocusImpl(const std::string&) override;

private:
    mem::Rc<DependencyContainer> dic;
    JoinGameDialog joinGameDialog;
    sf::Text text;
};

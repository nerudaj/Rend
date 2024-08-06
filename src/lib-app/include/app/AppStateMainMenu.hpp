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
    }

public:
    void input() override;

    void update() override {}

    void draw() override
    {
        dic->gui->draw();
    }

private:
    void goToGameSetup();

    void tryJoinGame(const std::string& ip);

    void buildLayout();

    void restoreFocusImpl(const std::string&) override;

private:
    mem::Rc<DependencyContainer> dic;
    JoinGameDialog joinGameDialog;
};

#pragma once

#include "utils/DependencyContainer.hpp"
#include <Memory.hpp>
#include <vector>

class [[nodiscard]] AppStateMainMenu final : public dgm::AppState
{
public:
    AppStateMainMenu(dgm::App& app, mem::Rc<DependencyContainer> dic)
        : dgm::AppState(
            app, dgm::AppStateConfig { .clearColor = sf::Color::White })
        , dic(dic)
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

    void buildLayout();

    void restoreFocusImpl(const std::string&) override;

private:
    mem::Rc<DependencyContainer> dic;
};

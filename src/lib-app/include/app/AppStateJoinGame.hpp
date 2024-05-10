#pragma once

#include <DGM/dgm.hpp>
#include <Memory.hpp>
#include <utils/DependencyContainer.hpp>

class [[nodiscard]] AppStateJoinGame final : public dgm::AppState
{
public:
    AppStateJoinGame(dgm::App& app, mem::Rc<DependencyContainer> dic)
        : dgm::AppState(app), dic(dic)
    {
        buildLayout();
    }

public:
    void input() override;

    void update() override {}

    void draw() override
    {
        dic->gui->draw();
    }

private:
    void buildLayout();

    void restoreFocusImpl(const std::string& message);

    void tryToJoin();

private:
    mem::Rc<DependencyContainer> dic;
    std::string ipAddressInput = "127.0.0.1";
};

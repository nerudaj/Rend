#pragma once

#include "utils/DependencyContainer.hpp"
#include <Client.hpp>
#include <DGM/dgm.hpp>
#include <SFML/Network.hpp>

class [[nodiscard]] AppStatePeerLobby final : public dgm::AppState
{
public:
    AppStatePeerLobby(
        dgm::App& app,
        mem::Rc<DependencyContainer> dic,
        const sf::IpAddress& hostAddress)
        : dgm::AppState(app), dic(dic), client(hostAddress, 10666)
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

    void restoreFocusImpl(const std::string& message) override;

private:
    mem::Rc<DependencyContainer> dic;
    Client client;
};
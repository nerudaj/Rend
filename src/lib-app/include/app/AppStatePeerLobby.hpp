#pragma once

#include "app/AppStateLobbyBase.hpp"
#include "utils/DependencyContainer.hpp"
#include <Client.hpp>
#include <DGM/dgm.hpp>
#include <SFML/Network.hpp>

class [[nodiscard]] AppStatePeerLobby final : public AppStateLobbyBase
{
public:
    AppStatePeerLobby(
        dgm::App& app,
        mem::Rc<DependencyContainer> dic,
        const sf::IpAddress& hostAddress);

public:
    void input() override;

    void update() override {}

    void draw() override
    {
        dic->gui->draw();
    }

private:
    void buildLayoutGameSetupImpl(tgui::Panel::Ptr target) override;

private:
    void restoreFocusImpl(const std::string& message) override;
};
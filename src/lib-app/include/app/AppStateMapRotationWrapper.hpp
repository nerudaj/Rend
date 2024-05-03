#pragma once

#include "Client.hpp"
#include "GameSettings.hpp"
#include "utils/DependencyContainer.hpp"
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <dgm/dgm.hpp>

import AppMessage;
import Memory;

class [[nodiscard]] AppStateMapRotationWrapper final : public dgm::AppState
{
public:
    AppStateMapRotationWrapper(
        dgm::App& app,
        mem::Rc<DependencyContainer> dic,
        mem::Rc<Client> client,
        GameOptions gameSettings,
        const std::string& mapPackName,
        const std::vector<std::string>& mapNames);

public:
    void input() override;

    void update() override {}

    void draw() override {}

private:
    void restoreFocusImpl(const std::string& message) override;
    std::optional<AppMessage> handleAppMessage(const AppMessage& message);

private:
    mem::Rc<DependencyContainer> dic;
    mem::Rc<Client> client;
    GameOptions gameSettings;
    std::string mapPackName;
    std::vector<std::string> mapNames;
    size_t currentMapIdx = 0;
};
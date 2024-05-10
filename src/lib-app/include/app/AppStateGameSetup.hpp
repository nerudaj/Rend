#pragma once

#include "Client.hpp"
#include "GameSettings.hpp"
#include "utils/DependencyContainer.hpp"
#include <Dialogs/MapPickerDialog.hpp>
#include <Memory.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <dgm/classes/AppState.hpp>
#include <dgm/classes/ResourceManager.hpp>

class [[nodiscard]] AppStateGameSetup final : public dgm::AppState
{
public:
    AppStateGameSetup(dgm::App& app, mem::Rc<DependencyContainer> dic) noexcept;

public:
    void input() override;

    void update() override {}

    void draw() override
    {
        dic->gui->draw();
    }

private:
    void restoreFocusImpl(const std::string& message) override;

    void buildLayout();

    void commitLobby();

    void handleNetworkUpdate(const ServerUpdateData& update);

    void startGame();

    [[nodiscard]] std::vector<PlayerOptions> createPlayerSettings() const;

    void selectMapPack(const std::string& packname);

    void selectMapPackAndSendUpdate(const std::string& packname);

    void openMapPicker();

    void handleMapRotationUpdate();

private:
    mem::Rc<DependencyContainer> dic;
    mem::Rc<Client> client;
    LobbySettings lobbySettings;
    std::vector<std::string> mapPackNames;
    mem::Box<MapPickerDialog> mapPickerDialog;
};

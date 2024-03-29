#pragma once

import Memory;
import Options;
import Audio;
import Input;
import Network;
import LobbySettings;

#include "GuiState.hpp"
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <dgm/classes/AppState.hpp>
#include <dgm/classes/ResourceManager.hpp>
#include <thread>

class [[nodiscard]] AppStateGameSetup final
    : public dgm::AppState
    , public GuiState
{
public:
    AppStateGameSetup(
        dgm::App& app,
        mem::Rc<const dgm::ResourceManager> resmgr,
        mem::Rc<tgui::Gui> gui,
        mem::Rc<AppOptions> settings,
        mem::Rc<AudioPlayer> audioPlayer,
        mem::Rc<Jukebox> jukebox,
        mem::Rc<PhysicalController> controller) noexcept;

public:
    void input() override;

    void update() override {}

    void draw() override
    {
        gui->draw();
    }

private:
    void restoreFocusImpl(const std::string& message) override
    {
        handleRestoreMessage(app, message);
        GuiState::restoreFocus(app.window.getWindowContext());
    }

    void buildLayoutImpl() override;

    void commitLobby();

    void handleNetworkUpdate(const ServerUpdateData& update);

    void startGame();

    [[nodiscard]] std::vector<PlayerOptions> createPlayerSettings() const;

private:
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<tgui::Gui> gui;
    mem::Rc<AppOptions> settings;
    mem::Rc<AudioPlayer> audioPlayer;
    mem::Rc<Jukebox> jukebox;
    mem::Rc<PhysicalController> controller;
    mem::Rc<Client> client;
    LobbySettings lobbySettings;
    std::vector<std::string> mapnames;
};

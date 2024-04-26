#pragma once

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <dgm/dgm.hpp>

import AppMessage;
import Audio;
import Memory;
import Network;
import Input;
import Options;

class [[nodiscard]] AppStateMapRotationWrapper final : public dgm::AppState
{
public:
    AppStateMapRotationWrapper(
        dgm::App& app,
        mem::Rc<const dgm::ResourceManager> resmgr,
        mem::Rc<tgui::Gui> gui,
        mem::Rc<AppOptions> settings,
        mem::Rc<AudioPlayer> audioPlayer,
        mem::Rc<Jukebox> jukebox,
        mem::Rc<PhysicalController> controller,
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
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<tgui::Gui> gui;
    mem::Rc<AppOptions> settings;
    mem::Rc<AudioPlayer> audioPlayer;
    mem::Rc<Jukebox> jukebox;
    mem::Rc<PhysicalController> controller;
    mem::Rc<Client> client;
    GameOptions gameSettings;
    std::string mapPackName;
    std::vector<std::string> mapNames;
    size_t currentMapIdx = 0;
};
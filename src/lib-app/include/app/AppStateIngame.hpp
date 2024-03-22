#pragma once

#include "engine/AiEngine.hpp"
#include "engine/AnimationEngine.hpp"
#include "engine/AudioEngine.hpp"
#include "engine/GameRulesEngine.hpp"
#include "engine/PhysicsEngine.hpp"
#include "engine/RenderingEngine.hpp"
#include <DGM/dgm.hpp>
#include <GameLoop.hpp>
#include <LevelD.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <core/Scene.hpp>
#include <events/EventQueue.hpp>
#include <utils/DemoFileHandler.hpp>
#include <utils/RollbackManager.hpp>

import Options;
import Memory;
import Audio;
import Input;
import Network;
import AppMessage;
import CoreTypes;

class [[nodiscard]] AppStateIngame final : public dgm::AppState
{
public:
    AppStateIngame(
        dgm::App& app,
        mem::Rc<const dgm::ResourceManager> resmgr,
        mem::Rc<tgui::Gui> gui,
        mem::Rc<AppOptions> settings,
        mem::Rc<AudioPlayer> audioPlayer,
        mem::Rc<Jukebox> jukebox,
        mem::Rc<PhysicalController> controller,
        mem::Rc<Client> client,
        GameOptions gameSettings,
        const LevelD& level,
        bool launchedFromEditor = false);

    ~AppStateIngame()
    {
        app.window.getWindowContext().setView(
            app.window.getWindowContext().getDefaultView());
        unlockMouse();
        client->sendMapEnded();
    }

public:
    virtual void input() override;
    virtual void update() override;
    virtual void draw() override;

private:
    void restoreFocusImpl(const std::string& message) override
    {
        if (auto&& msg = deserializeAppMessage(message); msg.has_value())
        {
            std::visit(
                overloaded { [&](const PopIfNotMainMenu&)
                             { app.popState(message); },
                             [](const PopIfPause&) {} },
                msg.value());
        }

        app.window.getWindowContext().setFramerateLimit(60);
        propagateSettings();
        lockMouse();
    }

    struct FrameState
    {
        std::size_t tick;
        dgm::DynamicBuffer<Entity> things;
        dgm::DynamicBuffer<Marker> markers;
        std::vector<InputSchema> inputs;
        std::vector<PlayerState> states;
        EntityIndexType cameraAnchorIdx;
    };

    void handleNetworkUpdate(const ServerUpdateData& update);
    [[nodiscard]] FrameState snapshotInputsIntoNewFrameState();
    void simulateFrameFromState(const FrameState& state, bool skipAudio);
    void evaluateWinCondition();
    void restoreState(const FrameState& state);
    void updateEngines();
    void processEvents(bool skipAudio);
    void backupState(FrameState& state);

    void lockMouse();
    void unlockMouse();

    void createPlayers();
    void propagateSettings();

    mem::Box<GameLoop> createGameLoop();

protected:
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<tgui::Gui> gui;
    mem::Rc<AppOptions> settings;
    GameOptions gameSettings;
    mem::Rc<AudioPlayer> audioPlayer;
    mem::Rc<Jukebox> jukebox;
    mem::Rc<PhysicalController> controller;
    mem::Rc<Client> client;
    mem::Rc<EventQueue> eventQueue;
    bool launchedFromEditor;

    std::vector<mem::Rc<ControllerInterface>> inputs;
    Scene scene;
    RollbackManager<FrameState, 10> stateManager;
    mem::Box<GameLoop> gameLoop;
    DemoFileHandler demoFileHandler;
    mem::Box<dgm::Camera> camera;
    bool ready = false;
    size_t lastTick = {};
};
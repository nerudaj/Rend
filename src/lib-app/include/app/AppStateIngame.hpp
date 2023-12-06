#pragma once

#include "audio/AudioPlayer.hpp"
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
#include <input/ControllerInterface.hpp>
#include <input/InputSchema.hpp>
#include <utils/DemoFileHandler.hpp>
#include <utils/RoundRobinBuffer.hpp>

import Options;
import Memory;

class AppStateIngame final : public dgm::AppState
{
public:
    [[nodiscard]] AppStateIngame(
        dgm::App& app,
        mem::Rc<const dgm::ResourceManager> resmgr,
        mem::Rc<tgui::Gui> gui,
        mem::Rc<AppOptions> settings,
        mem::Rc<AudioPlayer> audioPlayer,
        GameOptions gameSettings,
        const LevelD& level,
        bool launchedFromEditor = false);

    ~AppStateIngame()
    {
        app.window.getWindowContext().setView(
            app.window.getWindowContext().getDefaultView());
        unlockMouse();
    }

public:
    virtual void input() override;
    virtual void update() override;
    virtual void draw() override;

    virtual [[nodiscard]] bool isTransparent() const noexcept override
    {
        return false;
    }

    virtual void restoreFocus() override
    {
        app.window.getWindowContext().setFramerateLimit(60);
        propagateSettings();
        lockMouse();
    }

private:
    struct FrameState
    {
        std::size_t tick;
        dgm::DynamicBuffer<Entity> things;
        dgm::DynamicBuffer<Marker> markers;
        std::vector<InputSchema> inputs;
        std::vector<PlayerState> states;
        EntityIndexType cameraAnchorIdx;
    };

    void snapshotInputs(FrameState& state);
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

    [[nodiscard]] RenderSettings getRenderSettings() const noexcept
    {
        return RenderSettings { .WIDTH = settings->display.resolution.width,
                                .HEIGHT = settings->display.resolution.height,
                                .useDitheredShadows =
                                    settings->display.useDitheredShadows };
    }

protected:
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<tgui::Gui> gui;
    mem::Rc<AppOptions> settings;
    GameOptions gameSettings;
    mem::Rc<AudioPlayer> audioPlayer;
    mem::Rc<EventQueue> eventQueue;
    bool launchedFromEditor;

    std::vector<mem::Rc<ControllerInterface>> inputs;
    Scene scene;
    RoundRobinBuffer<FrameState, 10> stateBuffer;
    mem::Box<GameLoop> gameLoop;
    DemoFileHandler demoFileHandler;
    mem::Box<dgm::Camera> camera;
};
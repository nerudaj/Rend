#pragma once

#include "Settings.hpp"
#include "audio/AudioPlayer.hpp"
#include "engine/AiEngine.hpp"
#include "engine/AnimationEngine.hpp"
#include "engine/AudioEngine.hpp"
#include "engine/GameRulesEngine.hpp"
#include "engine/PhysicsEngine.hpp"
#include "engine/RenderingEngine.hpp"
#include <DGM/dgm.hpp>
#include <GameSettings.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <core/Scene.hpp>
#include <input/ControllerInterface.hpp>
#include <input/InputSchema.hpp>
#include <utils/DemoFileHandler.hpp>
#include <utils/RoundRobinBuffer.hpp>

import Memory;

class AppStateIngame final : public dgm::AppState
{
public:
    [[nodiscard]] AppStateIngame(
        dgm::App& app,
        mem::Rc<const dgm::ResourceManager> resmgr,
        mem::Rc<tgui::Gui> gui,
        mem::Rc<Settings> settings,
        mem::Rc<AudioPlayer> audioPlayer,
        GameSettings gameSettings);

    ~AppStateIngame()
    {
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
    void restoreState(const FrameState& state);
    void updateEngines();
    void processEvents(bool skipAudio);
    void backupState(FrameState& state);

    void lockMouse();
    void unlockMouse();

    void createPlayers(const GameSettings& settings);

protected:
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<tgui::Gui> gui;
    mem::Rc<Settings> settings;
    mem::Rc<AudioPlayer> audioPlayer;

    const sf::Vector2f GAME_RESOLUTION;

    std::vector<mem::Rc<ControllerInterface>> inputs;
    Scene scene;
    RoundRobinBuffer<FrameState, 10> stateBuffer;
    AiEngine aiEngine;
    AnimationEngine animationEngine;
    AudioEngine audioEngine;
    GameRulesEngine gameRulesEngine;
    PhysicsEngine physicsEngine;
    RenderingEngine renderingEngine;
    DemoFileHandler demoFileHandler;
};
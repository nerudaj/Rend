#pragma once

#include "Settings.hpp"
#include "audio/AudioPlayer.hpp"
#include "engine/AnimationEngine.hpp"
#include "engine/AudioEngine.hpp"
#include "engine/GameRulesEngine.hpp"
#include "engine/PhysicsEngine.hpp"
#include "engine/RenderingEngine.hpp"
#include <DGM/dgm.hpp>
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
        mem::Rc<AudioPlayer> audioPlayer);

public:
    virtual void input() override;
    virtual void update() override;
    virtual void draw() override;

    virtual [[nodiscard]] bool isTransparent() const noexcept override
    {
        return false;
    }

    virtual void restoreFocus() override {}

private:
    struct FrameState
    {
        std::size_t frameId = 0;
        dgm::DynamicBuffer<Entity> things;
        dgm::DynamicBuffer<Marker> markers;
        InputSchema inputs[MAX_PLAYER_COUNT];
    };

    void snapshotInputs(FrameState& state);
    void simulateFrameFromState(const FrameState& state);
    void restoreState(const FrameState& state);
    void updateEngines();
    void processEvents();
    void backupState(FrameState& state);

protected:
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<tgui::Gui> gui;
    mem::Rc<Settings> settings;
    mem::Rc<AudioPlayer> audioPlayer;

    const sf::Vector2f GAME_RESOLUTION;

    Scene scene;
    RoundRobinBuffer<FrameState, 10> stateBuffer;
    AnimationEngine animationEngine;
    AudioEngine audioEngine;
    GameRulesEngine gameRulesEngine;
    PhysicsEngine physicsEngine;
    RenderingEngine renderingEngine;
    std::array<mem::Box<ControllerInterface>, MAX_PLAYER_COUNT> inputs;
    DemoFileHandler demoFileHandler;
};
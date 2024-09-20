#pragma once

#include "Client.hpp"
#include "GameSettings.hpp"
#include "engine/AiEngine.hpp"
#include "engine/AnimationEngine.hpp"
#include "engine/AudioEngine.hpp"
#include "engine/GameRulesEngine.hpp"
#include "engine/PhysicsEngine.hpp"
#include "engine/RenderingEngine.hpp"
#include "utils/DemoFileHandler.hpp"
#include "utils/DependencyContainer.hpp"
#include "utils/Framerate.hpp"
#include "utils/WinConditionStrategy.hpp"
#include <DGM/dgm.hpp>
#include <GameLoop.hpp>
#include <LevelD.hpp>
#include <Memory.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <core/Scene.hpp>
#include <events/EventQueue.hpp>
#include <utils/RollbackManager.hpp>

class [[nodiscard]] AppStateIngame final : public dgm::AppState
{
public:
    AppStateIngame(
        dgm::App& app,
        mem::Rc<DependencyContainer> dic,
        mem::Rc<Client> client,
        GameOptions gameSettings,
        const LevelD& level,
        bool launchedFromEditor = false);

    ~AppStateIngame()
    {
        app.window.getWindowContext().setView(
            app.window.getWindowContext().getDefaultView());
        unlockMouse();
        client->readIncomingPackets(
            [](auto) {}); // there is at least one packet in pipeline
    }

public:
    virtual void input() override;
    virtual void update() override;
    virtual void draw() override;

private:
    void restoreFocusImpl(const std::string& message) override;

    struct FrameState
    {
        std::size_t tick;
        dgm::DynamicBuffer<Entity> things;
        dgm::DynamicBuffer<Marker> markers;
        std::vector<InputSchema> inputs;
        std::vector<bool> confirmedInputs;
        std::vector<PlayerState> states;
        EntityIndexType cameraAnchorIdx;
    };

    void handleNetworkUpdate(const ServerUpdateData& update);
    void setCurrentFrameDelay(std::vector<size_t>&& oldestTicks);
    [[nodiscard]] FrameState snapshotInputsIntoNewFrameState();
    void simulateFrameFromState(const FrameState& state, bool skipAudio);
    void evaluateWinCondition();
    void restoreState(const FrameState& state);
    void backupState(FrameState& state);
    [[nodiscard]] bool isFrameConfirmed() const;

    void lockMouse();
    void unlockMouse();

    void createPlayers();
    void propagateSettings();

    mem::Box<GameLoop> createGameLoop();

protected:
    constexpr static const unsigned ROLLBACK_WINDOW_SIZE = 20u;

    mem::Rc<DependencyContainer> dic;
    GameOptions gameSettings;
    mem::Rc<Client> client;
    mem::Rc<EventQueue> eventQueue;
    bool launchedFromEditor;

    std::vector<mem::Rc<ControllerInterface>> inputs;
    Scene scene;
    RollbackManager<FrameState, ROLLBACK_WINDOW_SIZE> stateManager;
    std::unordered_map<size_t, std::unordered_map<PlayerIdxType, InputSchema>>
        futureInputs; // indexed by tick
    mem::Box<GameLoop> gameLoop;
#ifdef _DEBUG
    DemoFileHandler demoFileHandler;
#endif
    mem::Box<dgm::Camera> camera;
    bool ready = false;
    size_t lastTick = {};
    Framerate framerate = Framerate(FPS);
    std::chrono::milliseconds artificialFrameDelay = {};
    size_t tickToRollbackTo = {};
    unsigned humanPlayerCount = 0;
    IsPointLimitReached isPointLimitReached;
};
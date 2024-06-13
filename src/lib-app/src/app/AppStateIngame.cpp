#include "app/AppStateIngame.hpp"
#include "app/AppStatePaused.hpp"
#include "app/AppStateWinnerAnnounced.hpp"
#include "utils/AppMessage.hpp"
#include <builder/SceneBuilder.hpp>
#include <chrono>
#include <events/EventQueue.hpp>
#include <print>
#include <thread>

[[nodiscard]] static std::vector<mem::Rc<ControllerInterface>> createInputs(
    mem::Rc<PhysicalController> physicalController,
    const GameOptions& gameSettings)
{
    std::vector<mem::Rc<ControllerInterface>> inputs;
    for (auto&& ps : gameSettings.players)
    {
        switch (ps.kind)
        {
            using enum PlayerKind;
        case LocalHuman:
            inputs.push_back(physicalController);
            break;
        case LocalNpc:
        case RemoteHuman:
            inputs.push_back(mem::Rc<AiController>());
            break;
        }
    }
    return inputs;
}

AppStateIngame::AppStateIngame(
    dgm::App& _app,
    mem::Rc<DependencyContainer> _dic,
    mem::Rc<Client> _client,
    GameOptions gameSettings,
    const LevelD& level,
    bool launchedFromEditor)
    : dgm::AppState(_app)
    , dic(_dic)
    , gameSettings(gameSettings)
    , client(_client)
    , launchedFromEditor(launchedFromEditor)
    , inputs(createInputs(dic->controller, gameSettings))
    , scene(SceneBuilder::buildScene(level, gameSettings.players.size()))
    , eventQueue()
    , gameLoop(createGameLoop())
    , demoFileHandler(
          dic->settings->cmdSettings.demoFile,
          dic->settings->cmdSettings.playDemo ? DemoFileMode::Read
                                              : DemoFileMode::Write)
    , camera(
          sf::FloatRect(0.f, 0.f, 1.f, 1.f),
          sf::Vector2f(sf::Vector2u(
              dic->settings->display.resolution.width,
              dic->settings->display.resolution.height)))
{
    for (auto&& opts : gameSettings.players)
    {
        if (opts.kind != PlayerKind::LocalNpc) humanPlayerCount++;
    }

    lockMouse();
    createPlayers();
    dic->jukebox->playIngameSong();
    dic->logger->log(client->sendMapReadySignal());
}

void AppStateIngame::input()
{
    // Loop until F-N is confirmed
    do
    {
        dic->logger->log(client->readIncomingPackets(std::bind(
            &AppStateIngame::handleNetworkUpdate,
            this,
            std::placeholders::_1)));
    } while (!isFrameConfirmed());

    if (!hasFocus) return;

    sf::Event event;
    while (app.window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            app.exit();
        }
        else if (event.type == sf::Event::LostFocus)
        {
            unlockMouse();
        }
        else if (event.type == sf::Event::GainedFocus)
        {
            lockMouse();
        }
        else if (dic->controller->isEscapePressed())
        {
            if (launchedFromEditor)
                app.popState();
            else
            {
                unlockMouse();
                app.pushState<AppStatePaused>(dic);
            }
        }
        else if (dic->controller->shouldTakeScreenshot())
        {
            const auto&& succeeded =
                app.window.getScreenshot().saveToFile(std::format(
                    "./Rend_screenshot_{0:%F}_{0:%H}-{0:%M}-{0:%S}.png",
                    std::chrono::system_clock::now()));
            assert(succeeded);
        }
    }

    dic->controller->update();
}

void AppStateIngame::update()
{
    if (shouldSkipUpdate())
    {
        dic->logger->log("Skipping frame update (ready={})", ready);
        return;
    }

    stateManager.insert(snapshotInputsIntoNewFrameState(), lastTick);

    if (hasFocus)
        sf::Mouse::setPosition(
            sf::Vector2i(app.window.getSize() / 2u),
            app.window.getWindowContext());

    namespace ph = std::placeholders;

    stateManager.forEachItemFromOldestToNewest(
        std::bind(
            &AppStateIngame::simulateFrameFromState, this, ph::_1, ph::_2),
        std::bind(&AppStateIngame::backupState, this, ph::_1));

    ++lastTick;

    evaluateWinCondition();

    framerate.ensureFramerate(artificialFrameDelay);
}

void AppStateIngame::draw()
{
    app.window.getWindowContext().setView(camera->getCurrentView());
    gameLoop->renderTo(app.window);
}

void AppStateIngame::restoreFocusImpl(const std::string& message)
{
    handleAppMessage<decltype(this)>(app, message);

    propagateSettings();
    lockMouse();
}

void AppStateIngame::handleNetworkUpdate(const ServerUpdateData& update)
{
    ready = update.state == ServerState::GameInProgress;
    auto&& oldestTicks = std::vector<size_t>(
        update.clients.size(), std::numeric_limits<size_t>::max());

    dic->logger->log(
        "Peer: Update in tick {}. Frame time: {}",
        stateManager.getLastInsertedTick(),
        app.time.getDeltaTime());
    for (auto&& inputData : update.inputs)
    {
        dic->logger->log(
            "\tInput for tick {} from client {}",
            inputData.tick,
            inputData.clientId);

        if (stateManager.isTickTooOld(inputData.tick))
        {
            dic->logger->log("\t\tGot outdated input, exiting");
            app.popState(ExceptionGameDisconnected::serialize());
        }
        else if (stateManager.isTickTooNew(inputData.tick))
        {
            dic->logger->log(
                "\t\tTick is too new, inserting into futureInputs");
            futureInputs[inputData.tick][inputData.clientId] = inputData.input;
        }
        else
        {
            auto& frame = stateManager.get(inputData.tick);
            frame.inputs.at(inputData.clientId) = inputData.input;
            frame.confirmedInputs.at(inputData.clientId) = true;
            oldestTicks[inputData.clientId] =
                std::min(oldestTicks[inputData.clientId], inputData.tick);
        }
    }

    setCurrentFrameDelay(std::move(oldestTicks));
}

void AppStateIngame::setCurrentFrameDelay(std::vector<size_t>&& oldestTicks)
{
    artificialFrameDelay = {};
    for (auto&& tick : oldestTicks)
    {
        if (stateManager.isTickHalfwayTooOld(tick))
        {
            dic->logger->log(
                "\t\tSlowing down, one of the updated clients is lagging "
                "behind");
            artificialFrameDelay =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::microseconds(1'000'000 / FPS * 5));
        }
    }
}

AppStateIngame::FrameState AppStateIngame::snapshotInputsIntoNewFrameState()
{
    auto&& state = FrameState {
        .inputs = inputs
                  | std::views::transform(
                      [](mem::Rc<ControllerInterface>& i) -> InputSchema
                      {
                          i->update();
                          return i->getSnapshot();
                      })
                  | std::ranges::to<decltype(FrameState::inputs)>(),
        .confirmedInputs = std::vector<bool>(inputs.size(), false)
    };

    for (auto&& [playerIdx, input] : futureInputs[lastTick])
    {
        state.inputs.at(playerIdx) = input;
        state.confirmedInputs.at(playerIdx) = true;
    }
    futureInputs.erase(lastTick);

    if (!hasFocus)
    {
        state.inputs[client->getMyIndex()] = InputSchema {};
    }

    dic->logger->log(
        "Sending inputs for frame {} (client id {})",
        lastTick,
        client->getMyIndex());
    dic->logger->log(client->sendCurrentFrameInputs(lastTick, state.inputs));

    // NOTE: put code for demos here if applicable

    return state;
}

void AppStateIngame::simulateFrameFromState(
    const FrameState& state, bool skipAudio)
{
    restoreState(state);
    gameLoop->update(FRAME_TIME, app.time.getDeltaTime(), skipAudio);
    ++scene.tick;
}

void AppStateIngame::restoreState(const FrameState& state)
{
    scene.tick = state.tick;
    scene.things = state.things.clone(); // restore things
    scene.markers = state.markers.clone();
    scene.playerStates = state.states;
    for (unsigned i = 0; i < state.inputs.size(); ++i)
    {
        scene.playerStates[i].input.deserializeFrom(state.inputs[i]);
    }
    scene.camera.anchorIdx = state.cameraAnchorIdx;

    // rebuild spatial index
    scene.spatialIndex.clear();
    for (auto&& [thing, id] : scene.things)
        scene.spatialIndex.returnToLookup(id, thing.hitbox);
}

void AppStateIngame::evaluateWinCondition()
{
    if (gameLoop->isPointlimitReached(gameSettings.fraglimit))
    {
        if (hasFocus)
        {
            unlockMouse();
            app.pushState<AppStateWinnerAnnounced>(
                dic,
                client,
                gameSettings,
                scene.playerStates
                    | std::views::transform([](const PlayerState& state)
                                            { return state.inventory.score; })
                    | std::ranges::to<std::vector<int>>());
        }
        else
        {
            app.popState(PopIfPause::serialize());
        }
    }
}

void AppStateIngame::backupState(FrameState& state)
{
    state.tick = scene.tick;
    state.things = scene.things.clone();
    state.markers = scene.markers.clone();
    state.states = scene.playerStates;
    state.cameraAnchorIdx = scene.camera.anchorIdx;
}

bool AppStateIngame::shouldSkipUpdate() const
{
    auto&& confirmedInputs = stateManager.getLatestState().confirmedInputs;
    bool waitingForInputs = lastTick > 0
                            && humanPlayerCount > std::accumulate(
                                   confirmedInputs.begin(),
                                   confirmedInputs.end(),
                                   0u,
                                   [](unsigned acc, bool val) {
                                       return acc + static_cast<unsigned>(val);
                                   });

    return !ready;
    //    || waitingForInputs;
}

bool AppStateIngame::isFrameConfirmed() const
{
    constexpr const size_t WINDOW_SIZE = 1;
    if (lastTick < WINDOW_SIZE) return true;

    // NOTE: is last tick really in manager?
    const auto& confirmations =
        stateManager.get(lastTick - WINDOW_SIZE).confirmedInputs;

    const auto sumConfirmed = std::accumulate(
        confirmations.begin(),
        confirmations.end(),
        0u,
        [](unsigned acc, bool val)
        { return acc + static_cast<unsigned>(val); });

    return humanPlayerCount == sumConfirmed;
}

void AppStateIngame::lockMouse()
{
    auto& window = app.window.getWindowContext();
    window.setMouseCursorVisible(false);
    window.setMouseCursorGrabbed(true);
}

void AppStateIngame::unlockMouse()
{
    app.window.getWindowContext().setMouseCursorVisible(true);
    app.window.getWindowContext().setMouseCursorGrabbed(false);
}

void AppStateIngame::createPlayers()
{
    for (PlayerStateIndexType idx = 0; idx < gameSettings.players.size(); ++idx)
    {
        scene.markers.emplaceBack(MarkerDeadPlayer {
            .rebindCamera = gameSettings.players[idx].bindCamera,
            .stateIdx = idx });

        scene.playerStates.push_back(PlayerState {});
        scene.playerStates.back().inventory =
            SceneBuilder::getDefaultInventory(idx, 0);

        if (gameSettings.players[idx].bindCamera) scene.camera.anchorIdx = idx;
        if (gameSettings.players[idx].kind == PlayerKind::LocalNpc)
            scene.playerStates[idx].blackboard =
                AiBlackboard { .input = inputs[idx].castTo<AiController>(),
                               .personality = static_cast<AiPersonality>(idx),
                               .playerStateIdx = idx };
        else if (gameSettings.players[idx].kind != PlayerKind::LocalNpc)
        {
            scene.playerStates.back().autoswapOnPickup =
                gameSettings.players[idx].autoswapOnPickup;
        }
    }
}

void AppStateIngame::propagateSettings()
{
    camera = mem::Box<dgm::Camera>(
        sf::FloatRect(0.f, 0.f, 1.f, 1.f),
        sf::Vector2f(sf::Vector2u(
            dic->settings->display.resolution.width,
            dic->settings->display.resolution.height)));

    dic->controller->updateSettings(dic->settings->input);

    gameLoop = createGameLoop();
}

mem::Box<GameLoop> AppStateIngame::createGameLoop()
{
    return mem::Box<GameLoop>(
        scene,
        eventQueue,
        dic->resmgr,
        dic->audioPlayer,
        gameSettings.players
            | std::views::transform([](const PlayerOptions& opts)
                                    { return opts.name; })
            | std::ranges::to<std::vector>(),
        dic->settings->display);
}
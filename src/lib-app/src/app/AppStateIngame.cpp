#include "app/AppStateIngame.hpp"
#include "app/AppStatePaused.hpp"
#include "app/AppStateWinnerAnnounced.hpp"
#include "utils/AppMessage.hpp"
#include <builder/SceneBuilder.hpp>
#include <chrono>
#include <events/EventQueue.hpp>
#include <format>
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
#ifdef _DEBUG
    , demoFileHandler(
          dic->settings->cmdSettings.demoFile,
          dic->settings->cmdSettings.playDemo ? DemoFileMode::Read
                                              : DemoFileMode::Write)
#endif
    , camera(
          sf::FloatRect(0.f, 0.f, 1.f, 1.f),
          sf::Vector2f(sf::Vector2u(
              dic->settings->display.resolution.width,
              dic->settings->display.resolution.height)))
    , isPointLimitReached(
          WinConditionStrategy::getWinConditionStrategy(gameSettings))
{
    for (auto&& opts : gameSettings.players)
    {
        if (opts.kind != PlayerKind::LocalNpc) humanPlayerCount++;
    }

    lockMouse();
    createPlayers();
    dic->jukebox->playIngameSong();
    dic->logger->logOrError(
        lastTick, "Sending map ready signal", client->sendMapReadySignal());
}

void AppStateIngame::input()
{
    tickToRollbackTo = stateManager.getLastInsertedTick();
    auto&& result = client->readPacketsUntil(
        [&](auto data) { handleNetworkUpdate(data); },
        [&] { return isFrameConfirmed(); });

    if (!result)
    {
        dic->logger->error(
            lastTick,
            "Reading packets until frame confirmed failed. Confirmed inputs: "
            "{}. Message: {}",
            nlohmann::json(getFarthestConfirmedInputs()).dump(),
            result.error());
        app.popState(ExceptionGameDisconnected::serialize());
    }
    else
    {
        dic->logger->log(
            lastTick,
            "Frame {} confirmed",
            lastTick - (ROLLBACK_WINDOW_SIZE - 1));
    }

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
    if (!ready) return;

    stateManager.insert(snapshotInputsIntoNewFrameState(), lastTick);

    if (hasFocus)
        sf::Mouse::setPosition(
            sf::Vector2i(app.window.getSize() / 2u),
            app.window.getWindowContext());

    namespace ph = std::placeholders;

    dic->logger->log(
        lastTick,
        "update(): getLastInsertedTick: {}; tickToRollbackTo: {}",
        stateManager.getLastInsertedTick(),
        tickToRollbackTo);
    stateManager.forEachItemFromTick(
        tickToRollbackTo,
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

    humanPlayerCount = 0;
    for (const auto& [clientIdx, clientData] :
         std::views::enumerate(update.clients))
    {
        if (clientData.state != ClientState::Disconnected) [[likely]]
            ++humanPlayerCount;
        else
        {
            dic->logger->log(
                lastTick,
                "Client {} named {} is disconnected",
                clientIdx,
                clientData.userOpts.name);
        }
    }

    dic->logger->log(lastTick, "Found {} human players", humanPlayerCount);

    dic->logger->log(
        lastTick,
        "Peer: Update in tick {}. Frame time: {}",
        stateManager.getLastInsertedTick(),
        app.time.getDeltaTime());
    for (auto&& inputData : update.inputs)
    {
        tickToRollbackTo = std::min(tickToRollbackTo, inputData.tick);
        dic->logger->log(
            lastTick,
            "Input for tick {} from client {}; currently rollbacking to {}",
            inputData.tick,
            inputData.clientId,
            tickToRollbackTo);

        if (stateManager.isTickTooOld(inputData.tick)) [[unlikely]]
        {
            dic->logger->log(lastTick, "Got outdated input - exiting");
            app.popState(ExceptionGameDisconnected::serialize());
        }
        else if (stateManager.isTickTooNew(inputData.tick))
        {
            dic->logger->log(
                lastTick, "Tick is too new - inserting into futureInputs");
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
    for (auto&& [clientIdx, tick] : std::views::enumerate(oldestTicks))
    {
        if (stateManager.isTickHalfwayTooOld(tick)
            && clientIdx != client->getMyIndex())
        {
            dic->logger->log(
                lastTick,
                "Slowing down - client {} is lagging behind",
                clientIdx);
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

#ifdef _DEBUG
    if (dic->settings->cmdSettings.playDemo)
        state.inputs[client->getMyIndex()] =
            nlohmann::json::parse(demoFileHandler.getLine());
    else
        demoFileHandler.writeLine(
            nlohmann::json(state.inputs[client->getMyIndex()]).dump());
#endif

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

    dic->logger->ifError(
        lastTick,
        "Sending inputs for current tick",
        client->sendCurrentFrameInputs(lastTick, state.inputs));

    return state;
}

void AppStateIngame::simulateFrameFromState(
    const FrameState& state, bool skipAudio)
{
    restoreState(state);
    gameLoop->update(
        FRAME_TIME,
        app.time.getDeltaTime(),
        skipAudio,
        [&]
        {
            // This gets executed after aiEngine.update
            // propagate AI inputs to playerStates::input
            // so NPCs do their thing
            for (auto&& i : std::views::iota(firstNpcIdx, state.inputs.size()))
            {
                scene.playerStates[i].input.deserializeFrom(
                    scene.playerStates[i]
                        .blackboard.value()
                        .input->getSnapshot());
            }
        });
    ++scene.tick;
}

void AppStateIngame::restoreState(const FrameState& state)
{
    scene.tick = state.tick;
    scene.things = state.things.clone(); // restore things
    scene.markers = state.markers.clone();
    scene.playerStates = state.states;

    for (auto&& i : std::views::iota(0u, firstNpcIdx))
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
    if (isPointLimitReached(scene))
    {
        dic->logger->log(lastTick, "Point limit has been reached");

        if (hasFocus)
        {
            unlockMouse();
            app.pushState<AppStateWinnerAnnounced>(
                dic, client, gameSettings, scene);
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

bool AppStateIngame::isFrameConfirmed() const
{
    if (lastTick < (ROLLBACK_WINDOW_SIZE - 1)) return true;

    const auto& confirmations = getFarthestConfirmedInputs();

    const auto sumConfirmed = std::accumulate(
        confirmations.begin(),
        confirmations.end(),
        0u,
        [](unsigned acc, bool val)
        { return acc + static_cast<unsigned>(val); });

    return humanPlayerCount <= sumConfirmed;
}

void AppStateIngame::lockMouse()
{
    app.window.getWindowContext().setMouseCursorGrabbed(true);
}

void AppStateIngame::unlockMouse()
{
    app.window.getWindowContext().setMouseCursorGrabbed(false);
}

void AppStateIngame::createPlayers()
{
    for (const auto&& [idx, playerSettings] :
         std::views::enumerate(gameSettings.players))
    {
        scene.markers.emplaceBack(MarkerDeadPlayer {
            .rebindCamera = playerSettings.bindCamera,
            .stateIdx = static_cast<size_t>(idx),
        });

        scene.playerStates.push_back(PlayerState {
            .inventory = SceneBuilder::getDefaultInventory(
                idx, 0, getTeamBelonging(playerSettings.team)),
            .blackboard =
                playerSettings.kind == PlayerKind::LocalNpc
                    ? std::optional<AiBlackboard> { {
                          .input = inputs[idx].castTo<AiController>(),
                          .personality = static_cast<AiPersonality>(idx),
                          .playerStateIdx = static_cast<size_t>(idx),
                      } }
                    : std::nullopt,
            .autoswapOnPickup = playerSettings.kind == PlayerKind::LocalNpc
                                    ? false
                                    : playerSettings.autoswapOnPickup,
            .playerSkin = playerSettings.team,
        });

        if (playerSettings.bindCamera) scene.camera.anchorIdx = idx;
        if (playerSettings.kind != PlayerKind::LocalNpc) ++firstNpcIdx;
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
        gameSettings.gameMode,
        dic->settings->display,
        dic->settings->cmdSettings);
}

Team AppStateIngame::getTeamBelonging(Team teamPreference)
{
    return gameSettings.gameMode == GameMode::SingleFlagCtf ? teamPreference
                                                            : Team::None;
}

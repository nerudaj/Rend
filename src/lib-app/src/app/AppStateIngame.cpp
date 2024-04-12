#include "app/AppStateIngame.hpp"
#include "app/AppStatePaused.hpp"
#include "app/AppStateWinnerAnnounced.hpp"
#include <builder/SceneBuilder.hpp>
#include <events/EventQueue.hpp>

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
            inputs.push_back(mem::Rc<AiController>());
            break;
        case RemoteHuman:
            throw std::runtime_error("unsupported controller");
            break;
        }
    }
    return inputs;
}

AppStateIngame::AppStateIngame(
    dgm::App& _app,
    mem::Rc<const dgm::ResourceManager> _resmgr,
    mem::Rc<tgui::Gui> _gui,
    mem::Rc<AppOptions> _settings,
    mem::Rc<AudioPlayer> _audioPlayer,
    mem::Rc<Jukebox> _jukebox,
    mem::Rc<PhysicalController> _controller,
    mem::Rc<Client> _client,
    GameOptions gameSettings,
    const LevelD& level,
    bool launchedFromEditor)
    : dgm::AppState(_app)
    , resmgr(_resmgr)
    , gui(_gui)
    , settings(_settings)
    , gameSettings(gameSettings)
    , audioPlayer(_audioPlayer)
    , jukebox(_jukebox)
    , controller(_controller)
    , client(_client)
    , launchedFromEditor(launchedFromEditor)
    , inputs(createInputs(_controller, gameSettings))
    , scene(SceneBuilder::buildScene(level, gameSettings.players.size()))
    , eventQueue()
    , gameLoop(createGameLoop())
    , demoFileHandler(
          settings->cmdSettings.demoFile,
          settings->cmdSettings.playDemo ? DemoFileMode::Read
                                         : DemoFileMode::Write)
    , camera(
          sf::FloatRect(0.f, 0.f, 1.f, 1.f),
          sf::Vector2f(sf::Vector2u(
              settings->display.resolution.width,
              settings->display.resolution.height)))
{
    app.window.getWindowContext().setFramerateLimit(60);
    lockMouse();
    createPlayers();
    jukebox->playIngameSong();
    client->sendMapReadySignal();
}

void AppStateIngame::input()
{

    client->readIncomingPackets(std::bind(
        &AppStateIngame::handleNetworkUpdate, this, std::placeholders::_1));

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
        else if (controller->isEscapePressed())
        {
            if (launchedFromEditor)
                app.popState();
            else
            {
                unlockMouse();
                app.pushState<AppStatePaused>(
                    gui, resmgr, audioPlayer, jukebox, controller, settings);
            }
        }
        else if (controller->shouldTakeScreenshot())
        {
            const auto&& succeeded =
                app.window.getScreenshot().saveToFile(std::format(
                    "./Rend_screenshot_{0:%F}_{0:%H}-{0:%M}-{0:%S}.png",
                    std::chrono::system_clock::now()));
            assert(succeeded);
        }
    }

    controller->update();
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

    stateManager.forEachItemFromOldestToNewest(
        std::bind(
            &AppStateIngame::simulateFrameFromState, this, ph::_1, ph::_2),
        std::bind(&AppStateIngame::backupState, this, ph::_1));

    ++lastTick;

    evaluateWinCondition();
}

void AppStateIngame::draw()
{
    app.window.getWindowContext().setView(camera->getCurrentView());
    gameLoop->renderTo(app.window);
}

void AppStateIngame::handleNetworkUpdate(const ServerUpdateData& update)
{
    ready = update.peersReady;

    for (auto&& inputData : update.inputs)
    {
        stateManager.get(inputData.tick).inputs.at(inputData.clientId) =
            inputData.input;
    }
}

AppStateIngame::FrameState AppStateIngame::snapshotInputsIntoNewFrameState()
{
    auto&& state =
        FrameState { .inputs =
                         inputs
                         | std::views::transform(
                             [](mem::Rc<ControllerInterface>& i) -> InputSchema
                             {
                                 i->update();
                                 return i->getSnapshot();
                             })
                         | std::ranges::to<decltype(FrameState::inputs)>() };

    if (!hasFocus)
    {
        // FIXME: myClientId
        state.inputs[0] = InputSchema {};
    }

    client->sendUpdate(lastTick, state.inputs);

    // TODO: remove the following line. It just disables local input and routes
    // it through network
    state.inputs[0] = InputSchema {};

    // TODO: rework demos
    /*if (settings->cmdSettings.playDemo)
    {
        auto line = demoFileHandler.getLine();
        if (line.empty())
        {
            app.exit();
            return state;
        }
        state.inputs[0] = nlohmann::json::parse(line);
    }
    else
    {
        demoFileHandler.writeLine(nlohmann::json(state.inputs[0]).dump());
    }*/

    return state;
}

void AppStateIngame::simulateFrameFromState(
    const FrameState& state, bool skipAudio)
{
    restoreState(state);
    gameLoop->update(FRAME_TIME, skipAudio);
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
                resmgr,
                gui,
                audioPlayer,
                jukebox,
                controller,
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
        else if (gameSettings.players[idx].kind == PlayerKind::LocalHuman)
        {
            scene.playerStates.back().autoswapOnPickup =
                settings->input.autoswapOnPickup;
        }
    }
}

void AppStateIngame::propagateSettings()
{
    camera = mem::Box<dgm::Camera>(
        sf::FloatRect(0.f, 0.f, 1.f, 1.f),
        sf::Vector2f(sf::Vector2u(
            settings->display.resolution.width,
            settings->display.resolution.height)));

    controller->updateSettings(settings->input);

    gameLoop = createGameLoop();
}

mem::Box<GameLoop> AppStateIngame::createGameLoop()
{
    return mem::Box<GameLoop>(
        scene,
        eventQueue,
        resmgr,
        audioPlayer,
        gameSettings.players
            | std::views::transform([](const PlayerOptions& opts)
                                    { return opts.name; })
            | std::ranges::to<std::vector>(),
        settings->display);
}
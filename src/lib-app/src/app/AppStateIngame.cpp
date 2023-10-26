#include "app/AppStateIngame.hpp"
#include "app/AppStatePaused.hpp"
#include <events/EventQueue.hpp>
#include <input/NullController.hpp>
#include <input/PhysicalController.hpp>
#include <utils/SceneBuilder.hpp>

void AppStateIngame::input()
{
    sf::Event event;
    while (app.window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            app.exit();
        }
        else if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Escape)
            {
                unlockMouse();
                app.pushState<AppStatePaused>(gui, audioPlayer, settings);
            }
            else if (event.key.code == sf::Keyboard::F1)
            {
                EventQueue::add<EventRenderToggle>(
                    EventRenderToggle { false, true });
            }
            else if (event.key.code == sf::Keyboard::P)
            {
                for (auto&& thing : scene.things)
                {
                    if (thing.first.typeId == EntityType::Player
                        && scene.playerId != thing.second
                        && thing.second > scene.playerId)
                    {
                        scene.playerId = thing.second;
                        break;
                    }
                }
            }
        }
    }
}

void AppStateIngame::update()
{
    stateBuffer.pushBack(FrameState {});
    snapshotInputs(stateBuffer.last());
    sf::Mouse::setPosition(
        sf::Vector2i(app.window.getSize() / 2u), app.window.getWindowContext());

    unsigned howMuchToUnroll = 2u; // Can be more, based on network latency
    unsigned startIndex = stateBuffer.getSize() - howMuchToUnroll;
    // Excluding state pushed back earlier - that is the next
    // frame
    unsigned endIndex = stateBuffer.getSize() - 1u;
    for (unsigned i = startIndex; i < endIndex; ++i)
    {
        auto&& state = stateBuffer[i];
        simulateFrameFromState(state);
        ++scene.tick; // advancing frame
        // Write the simulated state into the next frame
        backupState(stateBuffer[i + 1u]);
    }

    if (stateBuffer.getSize() == 1u)
    { // nothing was simulated, nothing was backed up yet
        backupState(stateBuffer.last());
    }
}

void AppStateIngame::draw()
{
    // Rendering everything that is subject to world coordinate system
    app.window.getWindowContext().setView(scene.worldCamera.getCurrentView());
    renderingEngine.renderWorldTo(app.window);

    // Rendering stuff that uses screen coordinates
    app.window.getWindowContext().setView(scene.hudCamera.getCurrentView());
    renderingEngine.renderHudTo(app.window);
}

void AppStateIngame::snapshotInputs(FrameState& state)
{
    state.inputs = inputs
                   | std::views::transform(
                       [](const mem::Rc<ControllerInterface>& i) -> InputSchema
                       { return i->getSnapshot(); })
                   | std::ranges::to<decltype(state.inputs)>();

    if (settings->cmdSettings.playDemo)
    {
        auto line = demoFileHandler.getLine();
        if (line.empty())
        {
            app.exit();
            return;
        }
        state.inputs[0] = nlohmann::json::parse(line);
    }
    else
    {
        demoFileHandler.writeLine(nlohmann::json(state.inputs[0]).dump());
    }
}

void AppStateIngame::simulateFrameFromState(const FrameState& state)
{
    restoreState(state);
    updateEngines();
    processEvents();
    gameRulesEngine.deleteMarkedObjects();
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

    // rebuild spatial index
    scene.spatialIndex.clear();
    for (auto&& [thing, id] : scene.things)
        scene.spatialIndex.returnToLookup(id, thing.hitbox);
}

void AppStateIngame::updateEngines()
{
    aiEngine.update(FRAME_TIME);
    animationEngine.update(FRAME_TIME);
    audioEngine.update(FRAME_TIME);
    physicsEngine.update(FRAME_TIME);
    gameRulesEngine.update(FRAME_TIME);
    renderingEngine.update(FRAME_TIME);
}

void AppStateIngame::processEvents()
{
    // Animation, Physics and Game can produce GameEvent
    EventQueue::processEvents<AnimationEvent>(animationEngine);
    EventQueue::processEvents<PhysicsEvent>(physicsEngine);
    EventQueue::processEvents<GameEvent>(gameRulesEngine);

    // Audio can only produce Audio events
    EventQueue::processEvents<AudioEvent>(audioEngine);

    // Rendering can only produce Rendering events
    EventQueue::processEvents<RenderingEvent>(renderingEngine);

    // No events should be unprocessed
    assert(EventQueue::animationEvents.empty());
    assert(EventQueue::audioEvents.empty());
    assert(EventQueue::gameEvents.empty());
    assert(EventQueue::physicsEvents.empty());
    assert(EventQueue::renderingEvents.empty());
}

void AppStateIngame::backupState(FrameState& state)
{
    state.tick = scene.tick;
    state.things = scene.things.clone();
    state.markers = scene.markers.clone();
    state.states = scene.playerStates;
}

[[nodiscard]] static std::vector<mem::Rc<ControllerInterface>>
createInputs(const sf::Window& window, const GameSettings& gameSettings)
{
    std::vector<mem::Rc<ControllerInterface>> inputs;
    for (auto&& ps : gameSettings.players)
    {
        switch (ps.kind)
        {
            using enum PlayerKind;
        case LocalHuman:
            inputs.push_back(mem::Rc<PhysicalController>(window));
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
    mem::Rc<Settings> _settings,
    mem::Rc<AudioPlayer> _audioPlayer,
    GameSettings gameSettings)
    : dgm::AppState(_app)
    , resmgr(_resmgr)
    , gui(_gui)
    , settings(_settings)
    , audioPlayer(_audioPlayer)
    , GAME_RESOLUTION(sf::Vector2f(app.window.getSize()))
    , inputs(createInputs(_app.window.getWindowContext(), gameSettings))
    , scene(SceneBuilder::buildScene(*resmgr, GAME_RESOLUTION, gameSettings))
    , aiEngine(scene)
    , animationEngine(scene)
    , audioEngine(resmgr, audioPlayer)
    , gameRulesEngine(scene)
    , physicsEngine(scene)
    , renderingEngine(resmgr, scene)
    , demoFileHandler(
          settings->cmdSettings.demoFile,
          settings->cmdSettings.playDemo ? DemoFileMode::Read
                                         : DemoFileMode::Write)
{
    scene.worldCamera.setPosition(GAME_RESOLUTION / 2.f);
    scene.hudCamera.setPosition(GAME_RESOLUTION / 2.f);
    lockMouse();
    createPlayers(gameSettings);
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

void AppStateIngame::createPlayers(const GameSettings& gameSettings)
{
    for (PlayerStateIndexType i = 0; i < gameSettings.players.size(); ++i)
    {
        auto spawnPosition = gameRulesEngine.getBestSpawnPosition();
        auto idx = scene.things.emplaceBack(SceneBuilder::createPlayer(
            Position { spawnPosition },
            Direction { gameRulesEngine.getBestSpawnDirection(spawnPosition) },
            i));
        scene.playerStates.push_back(PlayerState {});
        scene.playerStates.back().inventory =
            SceneBuilder::getDefaultInventory(idx);

        if (gameSettings.players[i].bindCamera) scene.playerId = idx;
        if (gameSettings.players[i].kind == PlayerKind::LocalNpc)
            scene.playerStates[i].blackboard =
                AiBlackboard { .input = inputs[i].castTo<AiController>(),
                               .playerStateIdx = i };
    }
}
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
                app.pushState<AppStatePaused>(gui, audioPlayer, settings);
            }
            else if (event.key.code == sf::Keyboard::F1)
            {
                EventQueue::add<EventRenderToggle>(
                    EventRenderToggle { false, true });
            }
        }
    }
}

void AppStateIngame::update()
{
    stateBuffer.pushBack(FrameState {});
    snapshotInputs(stateBuffer.last());

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
    for (unsigned i = 0; i < MAX_PLAYER_COUNT; i++)
    {
        state.inputs[i] = inputs[i]->getSnapshot();
    }

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
    for (unsigned i = 0; i < MAX_PLAYER_COUNT; i++)
    {
        // restore inputs
        scene.playerStates[i] = state.states[i];
        scene.playerStates[i].input.deserializeFrom(state.inputs[i]);
    }

    // rebuild spatial index
    scene.spatialIndex.clear();
    for (auto&& [thing, id] : scene.things)
        scene.spatialIndex.returnToLookup(id, thing.hitbox);
}

void AppStateIngame::updateEngines()
{
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

    for (auto i = 0; i < MAX_PLAYER_COUNT; i++)
        state.states[i] = scene.playerStates[i];
}

AppStateIngame::AppStateIngame(
    dgm::App& _app,
    mem::Rc<const dgm::ResourceManager> _resmgr,
    mem::Rc<tgui::Gui> _gui,
    mem::Rc<Settings> _settings,
    mem::Rc<AudioPlayer> _audioPlayer)
    : dgm::AppState(_app)
    , resmgr(_resmgr)
    , gui(_gui)
    , settings(_settings)
    , audioPlayer(_audioPlayer)
    , GAME_RESOLUTION(sf::Vector2f(app.window.getSize()))
    , scene(SceneBuilder::buildScene(*resmgr, GAME_RESOLUTION, *settings))
    , animationEngine(scene)
    , audioEngine(resmgr, audioPlayer)
    , gameRulesEngine(scene)
    , physicsEngine(scene)
    , renderingEngine(resmgr, scene)
    , inputs({
          mem::Box<PhysicalController>(),
          mem::Box<NullController>(),
          mem::Box<NullController>(),
          mem::Box<NullController>(),
      })
    , demoFileHandler(
          settings->cmdSettings.demoFile,
          settings->cmdSettings.playDemo ? DemoFileMode::Read
                                         : DemoFileMode::Write)
{
    scene.worldCamera.setPosition(GAME_RESOLUTION / 2.f);
    scene.hudCamera.setPosition(GAME_RESOLUTION / 2.f);

    for (short i = 0; i < MAX_PLAYER_COUNT; i++)
    {
        auto idx = scene.things.emplaceBack(SceneBuilder::createPlayer(
            Position { scene.spawns[i] },
            Direction { sf::Vector2f { 1.f, 0.f } },
            i));
        scene.playerStates[i].inventory =
            SceneBuilder::getDefaultInventory(idx);

        if (i == 0) scene.playerId = idx;
    }
}

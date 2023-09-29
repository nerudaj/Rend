#include "app/AppStateIngame.hpp"
#include "app/AppStatePaused.hpp"
#include <events/EventQueue.hpp>
#include <input/NullController.hpp>
#include <input/PhysicalController.hpp>

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
    if (stateBuffer.getSize() == 0u)
    {
        stateBuffer.pushBack(
            FrameState { .frameId = 0, .things = scene.things.clone() });
        snapshotInputs(stateBuffer.last());
        return;
    }

    stateBuffer.pushBack(FrameState {});
    snapshotInputs(stateBuffer.last());

    unsigned howMuchToUnroll = 2u; // Can be more, based on network latency
    unsigned startIndex =
        stateBuffer.isEmpty() ? 0u : stateBuffer.getSize() - howMuchToUnroll;
    // Excluding state pushed back earlier - that is the next
    // frame
    unsigned endIndex = stateBuffer.isEmpty() ? 0u : stateBuffer.getSize() - 1u;
    for (unsigned i = startIndex; i < endIndex; ++i)
    {
        auto&& state = stateBuffer[i];
        restoreState(state);
        updateEngines();
        processEvents();
        ++scene.frameId; // advancing frame
        // Write the resulting simulated state into the next
        // frame
        backupState(stateBuffer[i + 1u]);
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
        std::string line;
        std::getline(demoStream, line);
        state.inputs[0] = nlohmann::json::parse(line);
    }
    else
    {
        demoStream << nlohmann::json(state.inputs[0]).dump() << "\n";
    }
}

void AppStateIngame::simulateFrameFromState(const FrameState& state)
{
    restoreState(state);
    updateEngines();
    processEvents();
}

void AppStateIngame::restoreState(const FrameState& state)
{
    scene.frameId = state.frameId;
    scene.things = state.things.clone(); // restore things
    for (unsigned i = 0; i < MAX_PLAYER_COUNT; i++)
    {
        // restore inputs
        scene.inputs[i].deserializeFrom(state.inputs[i]);
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
    state.frameId = scene.frameId;
    state.things = scene.things.clone();
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
    , scene(Scene::buildScene(*resmgr, GAME_RESOLUTION, *settings))
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
    , demoStream(
          settings->cmdSettings.demoFile,
          settings->cmdSettings.playDemo ? std::ios_base::in
                                         : std::ios_base::trunc)
{
    scene.worldCamera.setPosition(GAME_RESOLUTION / 2.f);
    scene.hudCamera.setPosition(GAME_RESOLUTION / 2.f);

    scene.playerId = scene.things.emplaceBack(Scene::createPlayer(
        Position { scene.spawns[0] },
        Direction { sf::Vector2f { 1.f, 0.f } },
        0));

    for (short i = 1; i < MAX_PLAYER_COUNT; i++)
    {
        scene.things.emplaceBack(Scene::createPlayer(
            Position { scene.spawns[i] },
            Direction { sf::Vector2f { 1.f, 0.f } },
            i));
    }
}

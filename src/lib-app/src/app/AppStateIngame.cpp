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
    FrameState stateToBackup;
    snapshotInputs(stateToBackup);

    if (stateBuffer.getSize() > 0) [[likely]]
    {
        simulateFrameFromState(stateBuffer.last());
    }

    backupState(std::move(stateToBackup));
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
        state.inputs[i] = InputSchema {
            .thrust = inputs[i]->getThrust(),
            .sidewardThrust = inputs[i]->getSidewardThrust(),
            .steer = inputs[i]->getSteer(),
        };
    }
}

void AppStateIngame::simulateFrameFromState(const FrameState& state)
{
    restoreState(state);
    runEnginesUpdate();
    processEvents();
}

void AppStateIngame::restoreState(const FrameState& state)
{
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

void AppStateIngame::runEnginesUpdate()
{
    audioEngine.update(app.time);
    physicsEngine.update(app.time);
    gameRulesEngine.update(app.time);
    renderingEngine.update(app.time);
}

void AppStateIngame::processEvents()
{
    EventQueue::processEvents<AudioEvent>(audioEngine);
    EventQueue::processEvents<PhysicsEvent>(physicsEngine);
    EventQueue::processEvents<GameEvent>(gameRulesEngine);
    EventQueue::processEvents<RenderingEvent>(renderingEngine);
}

void AppStateIngame::backupState(FrameState&& state)
{
    state.things = scene.things.clone();
    stateBuffer.pushBack(std::move(state));
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
{
    scene.worldCamera.setPosition(GAME_RESOLUTION / 2.f);
    scene.hudCamera.setPosition(GAME_RESOLUTION / 2.f);

    scene.playerId = scene.things.emplaceBack(Scene::createPlayer(
        Position { scene.spawns[0] },
        Direction { sf::Vector2f { 1.f, 0.f } },
        0));

    for (unsigned i = 1; i < MAX_PLAYER_COUNT; i++)
    {
        scene.things.emplaceBack(Scene::createPlayer(
            Position { scene.spawns[i] },
            Direction { sf::Vector2f { 1.f, 0.f } },
            i));
    }
}

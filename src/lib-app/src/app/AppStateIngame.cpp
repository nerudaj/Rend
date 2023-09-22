#include "app/AppStateIngame.hpp"
#include "app/AppStatePaused.hpp"
#include <events/EventQueue.hpp>
#include <input/PhysicalController.hpp>
#include <input/NullController.hpp>

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
    audioEngine.update(app.time);
    physicsEngine.update(app.time);
    gameRulesEngine.update(app.time);
    renderingEngine.update(app.time);

    // At the end of each update, process the event queue
    EventQueue::processEvents<AudioEvent>(audioEngine);
    EventQueue::processEvents<PhysicsEvent>(physicsEngine);
    EventQueue::processEvents<GameEvent>(gameRulesEngine);
    EventQueue::processEvents<RenderingEvent>(renderingEngine);
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
{
    scene.worldCamera.setPosition(GAME_RESOLUTION / 2.f);
    scene.hudCamera.setPosition(GAME_RESOLUTION / 2.f);

    auto&& body = dgm::Circle(scene.spawns[0], PLAYER_RADIUS);
    scene.playerId = scene.things.insert(
        Player(body,
                 sf::Vector2f{ 1.f, 0.f },
            mem::Rc<PhysicalController>(),
            SpriteId::PlayerA0),
        body);

    for (unsigned i = 1; i < scene.spawns.size(); i++)
    {
        auto&& body = dgm::Circle(scene.spawns[i], PLAYER_RADIUS);
        scene.things.insert(
            Player(
                body,
                sf::Vector2f { 1.f, 0.f },
                mem::Rc<NullController>(),
                SpriteId::PlayerA0),
            body);
    }
}

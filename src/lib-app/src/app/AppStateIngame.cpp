#include "app/AppStateIngame.hpp"
#include "app/AppStatePaused.hpp"
#include "app/AppStateWinnerAnnounced.hpp"
#include <builder/SceneBuilder.hpp>
#include <events/EventQueue.hpp>
#include <input/NullController.hpp>
#include <input/PhysicalController.hpp>

[[nodiscard]] static std::vector<mem::Rc<ControllerInterface>> createInputs(
    const sf::Window& window,
    const GameOptions& gameSettings,
    float mouseSensitivity)
{
    std::vector<mem::Rc<ControllerInterface>> inputs;
    for (auto&& ps : gameSettings.players)
    {
        switch (ps.kind)
        {
            using enum PlayerKind;
        case LocalHuman:
            inputs.push_back(
                mem::Rc<PhysicalController>(window, mouseSensitivity));
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
    GameOptions gameSettings,
    const LevelD& level,
    bool launchedFromEditor)
    : dgm::AppState(_app)
    , resmgr(_resmgr)
    , gui(_gui)
    , settings(_settings)
    , gameSettings(gameSettings)
    , audioPlayer(_audioPlayer)
    , launchedFromEditor(launchedFromEditor)
    , inputs(createInputs(
          _app.window.getWindowContext(),
          gameSettings,
          settings->input.mouseSensitivity))
    , scene(SceneBuilder::buildScene(level, gameSettings.players.size()))
    , gameLoop(scene, eventQueue, resmgr, audioPlayer, getRenderSettings())
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
}

void AppStateIngame::input()
{
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
        else if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Escape)
            {
                if (launchedFromEditor)
                    app.popState();
                else
                {
                    unlockMouse();
                    app.pushState<AppStatePaused>(gui, audioPlayer, settings);
                }
            }
            else if (event.key.code == sf::Keyboard::P)
            {
                for (auto&& thing : scene.things)
                {
                    if (thing.first.typeId == EntityType::Player
                        && scene.cameraAnchorIdx != thing.second
                        && thing.second > scene.cameraAnchorIdx)
                    {
                        scene.cameraAnchorIdx = thing.second;
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

    const unsigned howMuchToUnroll =
        10u; // Can be more, based on network latency
    const unsigned startIndex = stateBuffer.getSize() < howMuchToUnroll
                                    ? 0u
                                    : stateBuffer.getSize() - howMuchToUnroll;
    // Excluding state pushed back earlier - that is the next
    // frame
    const unsigned endIndex = stateBuffer.getSize() - 1u;
    for (unsigned i = startIndex; i < endIndex; ++i)
    {
        auto&& state = stateBuffer[i];
        simulateFrameFromState(state, i == endIndex - 1);
        ++scene.tick; // advancing frame
        // Write the simulated state into the next frame
        backupState(stateBuffer[i + 1u]);
    }

    if (stateBuffer.getSize() == 1u)
    { // nothing was simulated, nothing was backed up yet
        backupState(stateBuffer.last());
    }

    evaluateWinCondition();
}

void AppStateIngame::draw()
{
    app.window.getWindowContext().setView(camera->getCurrentView());
    gameLoop->renderTo(app.window);
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

void AppStateIngame::simulateFrameFromState(
    const FrameState& state, bool skipAudio)
{
    restoreState(state);
    gameLoop->update(FRAME_TIME, skipAudio);
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
    scene.cameraAnchorIdx = state.cameraAnchorIdx;

    // rebuild spatial index
    scene.spatialIndex.clear();
    for (auto&& [thing, id] : scene.things)
        scene.spatialIndex.returnToLookup(id, thing.hitbox);
}

void AppStateIngame::evaluateWinCondition()
{
    if (gameLoop->isPointlimitReached(gameSettings.fraglimit))
    {
        unlockMouse();
        app.pushState<AppStateWinnerAnnounced>(
            gui,
            audioPlayer,
            gameSettings,
            scene.playerStates
                | std::views::transform([](const PlayerState& state)
                                        { return state.inventory.score; })
                | std::ranges::to<std::vector<int>>());
    }
}

void AppStateIngame::backupState(FrameState& state)
{
    state.tick = scene.tick;
    state.things = scene.things.clone();
    state.markers = scene.markers.clone();
    state.states = scene.playerStates;
    state.cameraAnchorIdx = scene.cameraAnchorIdx;
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
    for (PlayerStateIndexType i = 0; i < gameSettings.players.size(); ++i)
    {
        scene.markers.emplaceBack(MarkerDeadPlayer {
            .rebindCamera = gameSettings.players[i].bindCamera,
            .stateIdx = i });

        scene.playerStates.push_back(PlayerState {});
        scene.playerStates.back().inventory =
            SceneBuilder::getDefaultInventory(i, 0);

        if (gameSettings.players[i].bindCamera) scene.cameraAnchorIdx = i;
        if (gameSettings.players[i].kind == PlayerKind::LocalNpc)
            scene.playerStates[i].blackboard =
                AiBlackboard { .input = inputs[i].castTo<AiController>(),
                               .playerStateIdx = i };
    }
}

void AppStateIngame::propagateSettings()
{
    camera = mem::Box<dgm::Camera>(
        sf::FloatRect(0.f, 0.f, 1.f, 1.f),
        sf::Vector2f(sf::Vector2u(
            settings->display.resolution.width,
            settings->display.resolution.height)));

    for (std::size_t idx = 0; idx < gameSettings.players.size(); idx++)
    {
        if (gameSettings.players[idx].kind == PlayerKind::LocalHuman)
        {
            inputs[idx].castTo<PhysicalController>()->setMouseSensitivity(
                settings->input.mouseSensitivity);
        }
    }

    gameLoop = mem::Box<GameLoop>(
        scene, eventQueue, resmgr, audioPlayer, getRenderSettings());
}

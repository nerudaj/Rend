#include <LevelD.hpp>
#include <app/AppStateIngame.hpp>
#include <app/AppStateMapRotationWrapper.hpp>

import Resources;

AppStateMapRotationWrapper::AppStateMapRotationWrapper(
    dgm::App& app,
    mem::Rc<const dgm::ResourceManager> resmgr,
    mem::Rc<tgui::Gui> gui,
    mem::Rc<AppOptions> settings,
    mem::Rc<AudioPlayer> audioPlayer,
    mem::Rc<Jukebox> jukebox,
    mem::Rc<PhysicalController> controller,
    mem::Rc<Client> client,
    GameOptions gameSettings,
    const std::string& mapPackName,
    const std::vector<std::string>& mapNames)
    : dgm::AppState(app)
    , resmgr(resmgr)
    , gui(gui)
    , settings(settings)
    , audioPlayer(audioPlayer)
    , jukebox(jukebox)
    , controller(controller)
    , client(client)
    , gameSettings(gameSettings)
    , mapPackName(mapPackName)
    , mapNames(mapNames)
{
}

void AppStateMapRotationWrapper::input()
{
    auto&& lvd = LevelD();
    lvd.loadFromFile(Filesystem::getFullLevelPath(
                         settings->cmdSettings.resourcesDir,
                         mapPackName,
                         mapNames[currentMapIdx])
                         .string());

    currentMapIdx = (currentMapIdx + 1) % mapNames.size();

    app.pushState<AppStateIngame>(
        resmgr,
        gui,
        settings,
        audioPlayer,
        jukebox,
        controller,
        client,
        gameSettings,
        lvd);
}

void AppStateMapRotationWrapper::restoreFocusImpl(const std::string& message)
{
    deserializeAppMessage(message).and_then(std::bind(
        &AppStateMapRotationWrapper::handleAppMessage,
        this,
        std::placeholders::_1));
}

std::optional<AppMessage>
AppStateMapRotationWrapper::handleAppMessage(const AppMessage& message)
{
    std::visit(
        overloaded { [&](const PopIfNotMainMenu&)
                     { app.popState(PopIfNotMainMenu::serialize()); },
                     [](const PopIfNotMapRotationWrapper&) {},
                     [](const PopIfPause&) {} },
        message);
    return message;
}

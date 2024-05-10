#include "GuiBuilder.hpp"
#include <Filesystem.hpp>
#include <LevelD.hpp>
#include <app/AppStateIngame.hpp>
#include <app/AppStateMapRotationWrapper.hpp>

AppStateMapRotationWrapper::AppStateMapRotationWrapper(
    dgm::App& app,
    mem::Rc<DependencyContainer> dic,
    mem::Rc<Client> client,
    GameOptions gameSettings,
    const std::string& mapPackName,
    const std::vector<std::string>& mapNames)
    : dgm::AppState(app)
    , dic(dic)
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
                         dic->settings->cmdSettings.resourcesDir,
                         mapPackName,
                         mapNames[currentMapIdx])
                         .string());

    currentMapIdx = (currentMapIdx + 1) % mapNames.size();

    app.pushState<AppStateIngame>(dic, client, gameSettings, lvd);
}

void AppStateMapRotationWrapper::restoreFocusImpl(const std::string& message)
{
    handleAppMessage<decltype(this)>(app, message);
}

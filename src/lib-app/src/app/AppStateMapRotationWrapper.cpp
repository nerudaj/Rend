#include "GuiBuilder.hpp"
#include "utils/AppMessage.hpp"
#include <Filesystem.hpp>
#include <LevelD.hpp>
#include <MapProperties.hpp>
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

    MapProperties info = parseMapProperties(lvd.metadata.description);
    if (info.mapCompat == MapCompatibility::Deathmatch
        && gameSettings.gameMode == GameMode::SingleFlagCtf)
    {
        app.popState(ExceptionCtfMapWrongCompat::serialize());
    }
    else
    {
        app.pushState<AppStateIngame>(dic, client, gameSettings, lvd);
    }
}

void AppStateMapRotationWrapper::restoreFocusImpl(const std::string& message)
{
    handleAppMessage<AppStateMapRotationWrapper>(app, message);
}

#include "GuiBuilder.hpp"
#include <LevelD.hpp>
#include <app/AppStateIngame.hpp>
#include <app/AppStateMapRotationWrapper.hpp>

import Resources;

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

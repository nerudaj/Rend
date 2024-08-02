#include "Dialogs/ErrorInfoDialog.hpp"
#include "Dialogs/YesNoCancelDialog.hpp"
#include "Server.hpp"
#include "Shortcuts/ShortcutEngine.hpp"
#include "logging/LoggerFactory.hpp"
#include "utils/Framerate.hpp"
#include "utils/ServerMapLoader.hpp"
#include <app/AppStateEditor.hpp>
#include <app/AppStateGameSetup.hpp>
#include <app/AppStateServerWrapper.hpp>
#include <core/Constants.hpp>

void serverLoop(
    ServerConfiguration config,
    bool enableDebug,
    std::atomic_bool& serverEnabled,
    std::filesystem::path resourcesDir)
{
    auto&& logger =
        LoggerFactory::createLogger(enableDebug, "./rend_server_log.txt");

    try
    {
        auto&& server = Server(
            config,
            ServerDependencies { .logger = logger,
                                 .mapLoader =
                                     mem::Rc<ServerMapLoader>(resourcesDir) });
        auto&& framerate = Framerate(FPS * 2);
        logger->log("Server loop started");

        while (serverEnabled)
        {
            server.update();
            framerate.ensureFramerate();
        }
    }
    catch (const std::exception& e)
    {
        logger->log("error: {}", e.what());
    }
}

AppStateServerWrapper::AppStateServerWrapper(
    dgm::App& app, mem::Rc<DependencyContainer> dic, ServerWrapperTarget target)
    : dgm::AppState(app)
    , dic(dic)
    , serverEnabled(true)
    , serverThread(
          serverLoop,
          ServerConfiguration { .port = 10666,
                                .maxClientCount = 4,
                                .acceptReconnects =
                                    target == ServerWrapperTarget::Editor },
          dic->settings->cmdSettings.enableDebug,
          std::ref(serverEnabled),
          dic->settings->cmdSettings.resourcesDir)
{
    switch (target)
    {
        using enum ServerWrapperTarget;
    case GameSetup:
        app.pushState<AppStateGameSetup>(dic);
        break;
    case Editor: {
        app.pushState<AppStateEditor>(
            dic,
            mem::Rc<ShortcutEngine>([dic]
                                    { return dic->gui->isAnyModalOpened(); }),
            mem::Rc<YesNoCancelDialog>(dic->gui),
            mem::Rc<ErrorInfoDialog>(dic->gui));
        break;
    }
    }
}

AppStateServerWrapper::~AppStateServerWrapper()
{
    serverEnabled = false;
    serverThread.join();
}

void AppStateServerWrapper::restoreFocusImpl(const std::string& message)
{
    handleAppMessage<decltype(this)>(app, message);
}
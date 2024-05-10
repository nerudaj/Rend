#include "Dialogs/ErrorInfoDialog.hpp"
#include "Dialogs/YesNoCancelDialog.hpp"
#include "Server.hpp"
#include "Shortcuts/ShortcutEngine.hpp"
#include <app/AppStateEditor.hpp>
#include <app/AppStateGameSetup.hpp>
#include <app/AppStateServerWrapper.hpp>

void serverLoop(Server server, std::atomic_bool& serverEnabled)
{
    while (serverEnabled)
    {
        server.update();
    }
}

AppStateServerWrapper::AppStateServerWrapper(
    dgm::App& app, mem::Rc<DependencyContainer> dic, ServerWrapperTarget target)
    : dgm::AppState(app)
    , dic(dic)
    , serverEnabled(true)
    , serverThread(
          serverLoop,
          Server(ServerConfiguration {
              .port = 10666,
              .maxClientCount = 4,
              .acceptReconnects = target == ServerWrapperTarget::Editor }),
          std::ref(serverEnabled))
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
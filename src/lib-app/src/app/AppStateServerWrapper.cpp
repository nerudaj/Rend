#include "Dialogs/ErrorInfoDialog.hpp"
#include "Dialogs/YesNoCancelDialog.hpp"
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
    dgm::App& app,
    mem::Rc<const dgm::ResourceManager> resmgr,
    mem::Rc<tgui::Gui> gui,
    mem::Rc<AppOptions> settings,
    mem::Rc<AudioPlayer> audioPlayer,
    mem::Rc<Jukebox> jukebox,
    mem::Rc<PhysicalController> controller,
    ServerWrapperTarget target)
    : dgm::AppState(app)
    , resmgr(resmgr)
    , gui(gui)
    , settings(settings)
    , audioPlayer(audioPlayer)
    , jukebox(jukebox)
    , controller(controller)
    , serverEnabled(true)
    , serverThread(
          serverLoop,
          Server(ServerConfiguration { .port = 10666, .maxClientCount = 4 }),
          std::ref(serverEnabled))
{
    switch (target)
    {
        using enum ServerWrapperTarget;
    case GameSetup:
        app.pushState<AppStateGameSetup>(
            resmgr, gui, settings, audioPlayer, jukebox, controller);
        break;
    case Editor: {
        auto _gui = mem::Rc<Gui>();
        app.pushState<AppStateEditor>(
            gui,
            _gui,
            resmgr,
            settings,
            audioPlayer,
            jukebox,
            controller,
            mem::Rc<ShortcutEngine>([_gui]
                                    { return _gui->isAnyModalOpened(); }),
            mem::Rc<YesNoCancelDialog>(_gui),
            mem::Rc<ErrorInfoDialog>(_gui));
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
    if (PopIfNotMainMenu::canDeserializeFrom(message))
    {
        app.popState(message);
    }
}
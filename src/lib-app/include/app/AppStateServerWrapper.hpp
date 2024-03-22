#include <DGM/dgm.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <atomic>
#include <thread>

import Memory;
import Audio;
import Network;
import Input;
import Options;
import AppMessage;

enum class [[nodiscard]] ServerWrapperTarget
{
    GameSetup,
    Editor
};

class [[nodiscard]] AppStateServerWrapper final : public dgm::AppState
{
public:
    AppStateServerWrapper(
        dgm::App& app,
        mem::Rc<const dgm::ResourceManager> resmgr,
        mem::Rc<tgui::Gui> gui,
        mem::Rc<AppOptions> settings,
        mem::Rc<AudioPlayer> audioPlayer,
        mem::Rc<Jukebox> jukebox,
        mem::Rc<PhysicalController> controller,
        ServerWrapperTarget target);

    ~AppStateServerWrapper();

public:
    virtual void input() override {}

    virtual void update() override
    {
        app.popState();
    }

    virtual void draw() override {}

private:
    void restoreFocusImpl(const std::string& message);

private:
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<tgui::Gui> gui;
    mem::Rc<AppOptions> settings;
    mem::Rc<AudioPlayer> audioPlayer;
    mem::Rc<Jukebox> jukebox;
    mem::Rc<PhysicalController> controller;
    std::atomic_bool serverEnabled;
    std::thread serverThread;
};
#include "utils/DependencyContainer.hpp"
#include <DGM/dgm.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <atomic>
#include <thread>

import Memory;
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
        mem::Rc<DependencyContainer> dic,
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
    mem::Rc<DependencyContainer> dic;
    std::atomic_bool serverEnabled;
    std::thread serverThread;
};
#pragma once

#include "utils/AppMessage.hpp"
#include "utils/DependencyContainer.hpp"
#include <DGM/dgm.hpp>

class InputHandler
{
public:
    static void handleUiStateInput(dgm::App& app, DependencyContainer& dic)
    {
        handleUiStateInputInternal(app, dic);
    }

    static void handleUiStateWithFocus(
        bool hasFocus, dgm::App& app, DependencyContainer& dic)
    {
        handleUiStateInputInternal(
            app, dic, InternalFlags { .skipBecauseNoFocus = !hasFocus });
    }

    static void handleUiStateInputWithoutGoBackOption(
        dgm::App& app, DependencyContainer& dic)
    {
        handleUiStateInputInternal(
            app, dic, InternalFlags { .handleGoBackCase = false });
    }

    static void handleSendReady(
        dgm::App& app, DependencyContainer& dic, mem::Rc<Client> client)
    {
        auto result = client->sendPeerReadySignal();
        if (!result)
        {
            dic.logger->log(result);
            app.popState(ExceptionServerOffline::serialize());
        }
    }

private:
    struct InternalFlags
    {
        bool skipBecauseNoFocus = false;
        bool handleGoBackCase = true;
    };

    static void handleUiStateInputInternal(
        dgm::App& app, DependencyContainer& dic, InternalFlags flags = {})
    {
        if (flags.skipBecauseNoFocus) return;

        sf::Event event;
        while (app.window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                app.exit();
            }
            else if (
                flags.handleGoBackCase && dic.controller->isEscapePressed())
            {
                app.popState();
            }

            dic.gui->gui.handleEvent(event);
        }

        dic.controller->update();
    }
};
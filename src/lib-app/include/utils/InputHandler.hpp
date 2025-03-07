#pragma once

#include "utils/AppMessage.hpp"
#include "utils/DependencyContainer.hpp"
#include <DGM/dgm.hpp>

class InputHandler
{
public:
    static inline void
    handleUiStateInput(dgm::App& app, DependencyContainer& dic)
    {
        handleUiStateInputInternal(app, dic);
    }

    static inline void handleUiStateWithFocus(
        bool hasFocus, dgm::App& app, DependencyContainer& dic)
    {
        handleUiStateInputInternal(
            app, dic, InternalFlags { .skipBecauseNoFocus = !hasFocus });
    }

    static inline void handleUiStateInputWithoutGoBackOption(
        dgm::App& app, DependencyContainer& dic)
    {
        handleUiStateInputInternal(
            app, dic, InternalFlags { .handleGoBackCase = false });
    }

    static void handleSendReady(
        dgm::App& app, DependencyContainer& dic, mem::Rc<Client> client)
    {
        auto result = client->sendPeerReadySignal();
        dic.logger->logOrError(0, "sendPeerReady", result);
        if (!result)
        {
            app.popState(ExceptionServerOffline::serialize());
        }
    }

private:
    struct [[nodiscard]] InternalFlags final
    {
        bool skipBecauseNoFocus = false;
        bool handleGoBackCase = true;
    };

    static void handleUiStateInputInternal(
        dgm::App& app, DependencyContainer& dic, InternalFlags flags = {})
    {
        if (flags.skipBecauseNoFocus) return;

        dic.virtualCursor->update(app.time.getDeltaTime());

        sf::Event event;
        while (app.window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                app.exit();
            }
            else if (dic.controller->isConfirmPressed())
            {
                emulateGuiClick(
                    dic.gui->gui,
                    sf::Mouse::getPosition(app.window.getWindowContext()));
            }
            else if (
                flags.handleGoBackCase
                && (dic.controller->isEscapePressed()
                    || dic.controller->isCancelPressed()))
            {
                app.popState();
            }

            dic.gui->gui.handleEvent(event);
        }
    }

    static void emulateGuiClick(tgui::Gui& gui, const sf::Vector2i& mousePos)
    {
        gui.handleEvent(sf::Event { .type = sf::Event::MouseButtonPressed,
                                    .mouseButton = { .button = sf::Mouse::Left,
                                                     .x = mousePos.x,
                                                     .y = mousePos.y } });
        gui.handleEvent(sf::Event { .type = sf::Event::MouseButtonReleased,
                                    .mouseButton = { .button = sf::Mouse::Left,
                                                     .x = mousePos.x,
                                                     .y = mousePos.y } });
    }
};